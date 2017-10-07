//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/Asset.hpp>
#include <XR/FileBuffer.hpp>
#include <XR/Worker.hpp>
#include <XR/Hash.hpp>
#include <map>

#ifdef XR_DEBUG
#define ENABLE_ASSET_BUILDING
#endif

namespace XR
{

//==============================================================================
const size_t kChunkSizeBytes = XR_KBYTES(16);

struct LoadJob : Worker::Job
{
  // data
  Asset::Ptr asset;

  // structors
  explicit LoadJob(Asset::Ptr const& a)
  : asset(a)
  {}

  ~LoadJob()
  {
    CloseHandle();
  }

  // general
  virtual void Start()
  {
    CloseHandle();
    m_path = Asset::Manager::GetAssetPath() / asset->GetDescriptor().ToPath();
  }

  virtual bool Process()
  {
    bool done = false;
    if (!m_hFile)
    {
      m_hFile = File::Open(m_path, "rb");
      done = m_hFile == nullptr;
      if (done)
      {
        XR_TRACE(Asset::Manager, ("Failed to open asset '%s'.", m_path.c_str()));
        asset->FlagError();
      }
    }

    if (!done && m_data.size() == 0)
    {
      auto size = File::GetSize(m_hFile);
      done = size == 0;
      if (done)
      {
        XR_TRACE(Asset::Manager, ("Asset '%s' is an empty file.", m_path.c_str()));
        asset->FlagError();
      }
      else
      {
        m_data.resize(size);
        m_nextWrite = m_data.data();
      }
    }

    if (!done)
    {
      const size_t nextChunkSize = std::min(kChunkSizeBytes, m_data.size() - (m_nextWrite - m_data.data()));
      const size_t readSize = File::Read(m_hFile, 1, nextChunkSize, m_nextWrite);
      done = readSize != nextChunkSize;
      if (done)
      {
        XR_TRACE(Asset::Manager, ("Failed to read %d bytes of asset '%s' @ %d of %d bytes",
          nextChunkSize, m_path.c_str(), m_nextWrite - m_data.data(), m_data.size()));
        asset->FlagError();
      }
      else
      {
        auto pos = m_nextWrite + readSize;
        m_nextWrite = pos;
        done = pos - m_data.data() == m_data.size();
        asset->OverrideFlags(Asset::LoadingFlag, Asset::LoadedFlag);
      }
    }

    return done;
  }

  std::vector<uint8_t> const& GetData() const
  {
    return m_data;
  }

private:
  // data
  FilePath              m_path;
  File::Handle          m_hFile = nullptr;
  std::vector<uint8_t>  m_data;
  uint8_t*              m_nextWrite = nullptr;

  // internal
  void CloseHandle()
  {
    if(m_hFile)
    {
      File::Close(m_hFile);
      m_hFile = nullptr;
    }
  }
};

//==============================================================================
struct
{
  // data
  FilePath path;

  std::map<uint32_t, Asset::Builder const*> builders;

  Allocator* allocator;

  Worker worker;

  // general
  bool Manage(Asset::Ptr a)
  {
    std::unique_lock<decltype(m_assetsLock)> lock(m_assetsLock);
    auto const& desc = a->GetDescriptor();
    auto iFind = m_assets.find(desc);
    bool success = iFind == m_assets.end();
    if(success)
    {
      m_assets.insert(iFind, { desc, a });
      a->OverrideFlags(Asset::UnmanagedFlag, 0);
    }
    return success;
  }

  Asset::Ptr FindManaged(Asset::DescriptorCore const& desc)
  {
    std::unique_lock<decltype(m_assetsLock)> lock(m_assetsLock);
    Asset::Ptr asset;
    auto iFind = m_assets.find(desc);
    if (iFind != m_assets.end())
    {
      asset = iFind->second;
    }
    return asset;
  }

  void RemoveManaged(Asset& a)
  {
    std::unique_lock<decltype(m_assetsLock)> lock(m_assetsLock);
    auto iFind = m_assets.find(a.GetDescriptor());
    if (iFind != m_assets.end())
    {
      iFind->second->OverrideFlags(0, Asset::UnmanagedFlag);
      m_assets.erase(iFind);
    }
  }

  void ClearManaged()
  {
    std::unique_lock<decltype(m_assetsLock)> lock(m_assetsLock);
    m_assets.clear();
  }

  void EnqueueJob(LoadJob& lj)
  {
    {
      std::unique_lock<decltype(m_pendingLock)> lock(m_pendingLock);
      m_pending.push_back(&lj);
    }

    worker.Enqueue(lj);
  }

  void UpdateJobs()
  {
    decltype(m_pending) q;
    {
      std::unique_lock<decltype(m_pendingLock)> lock(m_pendingLock);
      auto i = m_pending.begin();
      while (i != m_pending.end() &&
        IsFullMask((*i)->asset->GetFlags(), Asset::LoadedFlag))
      {
        ++i;
      }

      q.adopt(m_pending, i);
    }

    for (auto i0 = q.begin(), i1 = q.end(); i0 != i1; ++i0)
    {
      auto j = *i0;
      if (IsFullMask(j->asset->GetFlags(), Asset::LoadedFlag))
      {
        if (j->asset->OnLoaded(j->GetData().size(), j->GetData().data()))
        {
          j->asset->OverrideFlags(Asset::LoadedFlag, Asset::ReadyFlag);
        }
        else
        {
          j->asset->FlagError();
        }
      }

      DeleteJob(*j);
    }
  }

  void DeleteJob(LoadJob& lj)
  {
    lj.~LoadJob();
    allocator->Deallocate(&lj);
  }

private:
  // data
  Spinlock m_assetsLock;
  std::map<Asset::DescriptorCore, Asset::Ptr> m_assets;

  Spinlock m_pendingLock;
  Queue<LoadJob*> m_pending;

} s_assetMan;

//==============================================================================
void LoadAsset(Asset::Ptr const& asset, Asset::FlagType flags)
{
  if (IsFullMask(flags, Asset::LoadSyncFlag))
  {
    LoadJob lj(asset);
    lj.Start();
    while (!lj.Process())
    {}

    if (IsFullMask(lj.asset->GetFlags(), Asset::LoadedFlag))
    {
      if (lj.asset->OnLoaded(lj.GetData().size(), lj.GetData().data()))
      {
        lj.asset->OverrideFlags(Asset::LoadedFlag, Asset::ReadyFlag);
      }
      else
      {
        lj.asset->FlagError();
      }
    }
  }
  else
  {
    auto lj = new (s_assetMan.allocator->Allocate(sizeof(LoadJob))) LoadJob(asset);
    s_assetMan.EnqueueJob(*lj);
  }
}

//==============================================================================
char const* const Asset::Manager::kDefaultPath = "assets";

//==============================================================================
void Asset::Manager::Init(FilePath path, Allocator* alloc)
{
  if(path.StartsWith(File::kRawProto))
  {
    path = path.c_str() + File::kRawProto.size();
  }

  path = File::StripRoots(path);
  if (path.empty())
  {
    path = kDefaultPath;
  }
  s_assetMan.path.AppendDirSeparator();

#ifdef ENABLE_ASSET_BUILDING
  File::MakeDirs(s_assetMan.path);
#endif

  if (!alloc)
  {
    static Mallocator mallocator;
    alloc = &mallocator;
  }

  s_assetMan.allocator = alloc;
}

//==============================================================================
void Asset::Manager::RegisterBuilder(Builder const& builder)
{
#ifdef ENABLE_ASSET_BUILDING
  auto exts = builder.GetExtensions();
  while (exts)
  {
    auto next = strchr(exts, ';');
    size_t size = next ? next - exts : strlen(exts);
    if (size > 0)
    {
      const uint32_t hash = Hash::String32(exts, size);
      auto iFind = s_assetMan.builders.find(hash);
      if (iFind == s_assetMan.builders.end())
      {
        s_assetMan.builders.insert(iFind, { hash, &builder });
      }
      else
      {
        XR_ERROR(("Builder clash on extension '%s'", std::string(exts, size).c_str()));
      }
    }
    exts = next;
  }
#endif  // ENABLE_ASSET_BUILDING
}

//==============================================================================
const FilePath & Asset::Manager::GetAssetPath()
{
  return s_assetMan.path;
}

//==============================================================================
uint64_t Asset::Manager::HashPath(FilePath path)
{
  path = File::StripRoots(path);
  // Strip the asset path too, as the user is not required to specify it to
  // operate on assets.
  if (path.StartsWith(s_assetMan.path))
  {
    path = FilePath(path.c_str() + s_assetMan.path.size());
  }

  // We have a built asset if the remaining path is in our built asset path format:
  // - no extension
  // - 16 digits hex name
  // - in a two-deep directory structure, the parent directories being the first
  //   and second characters of the [hashed] name.
  char const* nameExt = path.GetNameExt();
  char const* ext = path.GetExt();
  uint64_t hash;
  if (!ext &&
    nameExt == path.data() + 4 &&
    path.size() == 20 &&
    sscanf(nameExt, "%llx", &hash) == 1)
  {
    if (DescriptorCore(0, hash).ToPath() != path)
    {
      hash = 0;
    }
  }
  else
  {
    // Otherwise drop the extension and hash what's left, which must not be an empty string.
    const size_t size = ext ? ext - path.c_str() : path.size();
    if (size > 0)
    {
      hash = Hash::String(path.c_str(), size, true);
    }
    else
    {
      hash = 0;
    }
  }

  if (hash == 0)
  {
    XR_ERROR(("'%s' is not a valid asset path.", path.c_str()));
  }
  return hash;
}

//==============================================================================
Asset::Ptr Asset::Manager::Find(DescriptorCore const& desc)
{
  return s_assetMan.FindManaged(desc);
}

//==============================================================================
bool Asset::Manager::Manage(Asset::Ptr asset)
{
  bool success = s_assetMan.Manage(asset);
  return success;
}

//==============================================================================
bool Asset::Manager::IsLoadable(FlagType oldFlags, FlagType newFlags)
{
  return !(IsFullMask(oldFlags, LoadingFlag) ||
    (IsFullMask(oldFlags, LoadedFlag) && !IsFullMask(newFlags, ForceReloadFlag)));
}

//==============================================================================
void Asset::Manager::LoadInternal(FilePath const& path, Asset::Ptr const& asset,
  FlagType flags)
{
#ifdef XR_DEBUG
  // Store the original path for debugging purposes
  asset->m_debugPath = path.c_str();
#endif

  // Clear private flags and set Loading
  asset->OverrideFlags(PrivateMask, LoadingFlag);

#ifdef ENABLE_ASSET_BUILDING
  // Check the name -- are we trying to load a raw or a built asset?
  // Strip ram/rom and asset roots.
  FilePath assetPath(File::StripRoots(path));
  if (assetPath.StartsWith(s_assetMan.path))
  {
    assetPath = FilePath(assetPath.c_str() + s_assetMan.path.size());
  }

  FilePath finalPath = asset->GetDescriptor().ToPath();
  if (finalPath != assetPath)
  {
    // Check for raw and built asset, compare last modification time.
    auto tsRaw = File::GetModifiedTime(path); // must use original path!
    XR_ASSERTMSG(Asset::Manager, tsRaw > 0, ("'%s' doesn't exist.", path.c_str()));
    auto tsBuilt = File::GetModifiedTime(s_assetMan.path / finalPath.c_str());

    // If built asset doesn't exist or is older, then rebuild it.
    if(tsBuilt < tsRaw)
    {
      auto ext = assetPath.GetExt();
      XR_ASSERT(Asset::Manager, ext);

      uint32_t hash = Hash::String32(ext);
      auto iFind = s_assetMan.builders.find(hash);
      bool done = iFind == s_assetMan.builders.end();
      if (done)
      {
        XR_TRACE(Asset::Manager, ("Failed to find builder for '%s'.", ext));
        asset->FlagError();
      }

      FileBuffer fb;
      if (!done)  // read source file 
      {
        done = !fb.Open(path, false);
        if (done)
        {
          XR_TRACE(Asset::Manager, ("Failed to read source '%s'.", path.c_str()));
          asset->FlagError();
        }
        fb.Close();
      }

      FilePath pathBuilt;
      if (!done)  // build asset
      {
        pathBuilt = File::GetRamPath() / s_assetMan.path.c_str() / finalPath.c_str();
        done = !File::MakeDirs(pathBuilt);
        if (done)
        {
          XR_TRACE(Asset::Manager, ("Failed to create directory structure for built asset '%s'",
            pathBuilt.c_str()));
        }
      }

      if (!done)
      {
        done = !iFind->second->Build(fb.GetData(), fb.GetSize(), pathBuilt);
        if (done)
        {
          XR_TRACE(Asset::Manager, ("Failed to build asset '%s'.", pathBuilt.c_str()));
          asset->FlagError();
        }
      }
    }
  }

  // If we're flawless, process job.
  if (!IsFullMask(asset->GetFlags(), ErrorFlag))
#endif  // ENABLE_ASSET_BUILDING
  {
    LoadAsset(asset, flags);
  }
}

//==============================================================================
void Asset::Manager::LoadInternal(Ptr const& asset, FlagType flags)
{
#ifdef XR_DEBUG
  // Clear the debug path -- the descriptor tells you where the asset is.
  asset->m_debugPath.clear();
#endif

  // Clear private flags and set Loading
  asset->OverrideFlags(PrivateMask, LoadingFlag);

  LoadAsset(asset, flags);
}

//==============================================================================
bool Asset::Manager::Unload(Asset& asset)
{
  if (!IsFullMask(asset.GetFlags(), UnmanagedFlag))
  {
    s_assetMan.RemoveManaged(asset);
  }
  return false;
}

//==============================================================================
void Asset::Manager::Update()
{
  s_assetMan.UpdateJobs();
}

//==============================================================================
void Asset::Manager::Suspend()
{
  s_assetMan.worker.Suspend();
}

//==============================================================================
void Asset::Manager::Resume()
{
  s_assetMan.worker.Resume();
}

//==============================================================================
void Asset::Manager::Exit()
{
  s_assetMan.worker.CancelPendingJobs();
  s_assetMan.worker.Finalize();

  s_assetMan.ClearManaged();
}

} // XR

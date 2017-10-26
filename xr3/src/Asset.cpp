//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Asset.hpp"
#include "XR/FileBuffer.hpp"
#include "XR/FileWriter.hpp"
#include "XR/Worker.hpp"
#include "XR/Hash.hpp"
#include "XR/ScopeGuard.hpp"
#include <map>

namespace XR
{

//==============================================================================
struct AssetHeader
{
  uint32_t typeId;
  uint16_t version;
  uint16_t reserved;
};

//==============================================================================
const size_t kChunkSizeBytes = XR_KBYTES(16);

struct LoadJob : Worker::Job
{
  // data
  Asset::Ptr asset;

  // structors
  explicit LoadJob(uint16_t acceptVersion, Asset::Ptr const& a)
  : asset(a),
    m_version(acceptVersion)
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
    if (!m_hFile) // Open file.
    {
      m_hFile = File::Open(m_path, "rb");
      done = m_hFile == nullptr;
      if (done)
      {
        XR_TRACE(Asset::Manager, ("Failed to open asset '%s'.", m_path.c_str()));
        asset->FlagError();
      }
    }

    if (!done && m_data.empty())  // get size
    {
      auto size = File::GetSize(m_hFile);
      AssetHeader header;
      done = size < sizeof(AssetHeader);
      if (done)
      {
        XR_TRACE(Asset::Manager, ("Asset '%s' doesn't have a valid header.",
          m_path.c_str()));
        asset->FlagError();
      }
      else // read header
      {
        size -= sizeof(header);

        done = File::Read(m_hFile, sizeof(header), 1, &header) < 1;
        if (done)
        {
          XR_TRACE(Asset::Manager, ("Failed to read header of asset %s",
            m_path.c_str()));
          asset->FlagError();
        }
      }

      if (!done)  // check typeId
      {
        auto type = asset->GetDescriptor().type;
        done = header.typeId != type;
        if (done)
        {
          XR_TRACE(Asset::Manager, ("Asset version mismatch, expected: %d, got: %d",
            type, header.typeId));
          asset->FlagError();
        }
      }

      if (!done)  // check version
      {
        done = header.version != m_version;
        if (done)
        {
          XR_TRACE(Asset::Manager, ("Asset version mismatch, expected: %d, got: %d",
            m_version, header.version));
          asset->FlagError();
        }
      }

      if (!done)  // allocate buffer
      {
        m_data.resize(size);
        m_nextWrite = m_data.data();
      }
    }

    if (!done)  // read file chunk by chunk
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

  bool Load()
  {
    return asset->Load(m_data.size(), m_data.data());
  }

private:
  // data
  uint16_t              m_version;
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

  bool RemoveManaged(Asset& a)
  {
    std::unique_lock<decltype(m_assetsLock)> lock(m_assetsLock);
    auto iFind = m_assets.find(a.GetDescriptor());
    bool success = iFind != m_assets.end();
    if (success)
    {
      iFind->second->OverrideFlags(0, Asset::UnmanagedFlag);
      m_assets.erase(iFind);
    }
    return success;
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
        CheckAllMaskBits((*i)->asset->GetFlags(), Asset::LoadedFlag))
      {
        ++i;
      }

      q.adopt(m_pending, i);
    }

    for (auto i0 = q.begin(), i1 = q.end(); i0 != i1; ++i0)
    {
      auto j = *i0;
      if (CheckAllMaskBits(j->asset->GetFlags(), Asset::LoadedFlag))
      {
        j->Load();
      }

      DeleteJob(*j);
    }
  }

  void DeleteJob(LoadJob& lj)
  {
    lj.~LoadJob();
    allocator->Deallocate(&lj);
  }

  void UnloadUnused()
  {
    std::unique_lock<decltype(m_assetsLock)> lock(m_assetsLock);
    for (auto& i: m_assets)
    {
      if(i.second->GetRefCount() == 1)
      {
        i.second->Unload();
      }
    }
  }

private:
  // data
  Spinlock m_assetsLock;
  std::map<Asset::DescriptorCore, Asset::Ptr> m_assets;

  Spinlock m_pendingLock;
  Queue<LoadJob*> m_pending;

} s_assetMan;

//==============================================================================
static void LoadAsset(uint16_t version, Asset::Ptr const& asset, Asset::FlagType flags)
{
  if (CheckAllMaskBits(flags, Asset::LoadSyncFlag))
  {
    LoadJob lj(version, asset);
    lj.Start();
    while (!lj.Process())
    {}

    if (CheckAllMaskBits(lj.asset->GetFlags(), Asset::LoadedFlag))
    {
      lj.Load();
    }
  }
  else
  {
    auto lj = new (s_assetMan.allocator->Allocate(sizeof(LoadJob))) LoadJob(version, asset);
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

      // If there's no conflict on an extension, or the pre-existing Builder is
      // Overridable(), then go ahead and register the new one.
      if (iFind == s_assetMan.builders.end() || iFind->second->Overridable())
      {
        s_assetMan.builders.insert(iFind, { hash, &builder });
      }
      else if(!builder.Overridable())
      {
        // If we've had a non-overridable builder, then registering another one is an error.
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
  XR_ASSERT(Asset::Manager, asset);
  bool success = s_assetMan.Manage(asset);
  return success;
}

//==============================================================================
bool Asset::Manager::IsLoadable(FlagType oldFlags, FlagType newFlags)
{
  return !(CheckAllMaskBits(oldFlags, LoadingFlag) ||
    (CheckAllMaskBits(oldFlags, LoadedFlag) && !CheckAllMaskBits(newFlags, ForceReloadFlag)));
}

//==============================================================================
#ifdef ENABLE_ASSET_BUILDING
static void BuildAsset(uint16_t version, FilePath const& path, Asset::Ptr const& asset)
{
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
    bool rebuild = tsBuilt < tsRaw;
    if (!rebuild) // if built asset exists and isn't older, then check version
    {
      auto hFile = File::Open(finalPath, "rb");
      auto guard = MakeScopeGuard([&hFile] {
        if (hFile)
        {
          File::Close(hFile);
        }
      });

      uint16_t versionFound;
      // Not being able to open the file (which we know for sure exists), to
      // seek 4 bytes and to read a version number isn't a hard error -- we'll
      // just have to rebuild, and we can deal with any persistent I/O errors
      // at that point.
      rebuild = !(hFile &&
        File::Seek(hFile, sizeof(AssetHeader::typeId), XR::File::SeekFrom::Start) &&
        File::Read(hFile, sizeof(versionFound), 1, &versionFound) == 1 &&
        versionFound == version);
    }

    if (rebuild)
    {
      auto ext = assetPath.GetExt();
      XR_ASSERT(Asset::Manager, ext);

      uint32_t hash = Hash::String32(ext);
      auto iFind = s_assetMan.builders.find(hash);
      bool done = iFind == s_assetMan.builders.end();
      if (done)
      {
        XR_TRACE(Asset::Manager, ("Failed to find builder for '%s'.", ext));
      }

      FileBuffer fb;
      if (!done)  // read source file 
      {
        done = !fb.Open(path, false);
        if (done)
        {
          XR_TRACE(Asset::Manager, ("Failed to read source '%s'.", path.c_str()));
        }
        fb.Close();
      }

      FilePath pathBuilt;
      if (!done)  // make asset directory if need be
      {
        pathBuilt = File::GetRamPath() / s_assetMan.path.c_str() / finalPath.c_str();
        done = !File::MakeDirs(pathBuilt);
        if (done)
        {
          XR_TRACE(Asset::Manager, ("Failed to create directory structure for built asset '%s'",
            pathBuilt.c_str()));
        }
      }

      FileWriter assetWriter;
      if (!done)  // create asset file
      {
        done = !assetWriter.Open(pathBuilt, FileWriter::Mode::Truncate, false);
        if (done)
        {
          XR_TRACE(Asset::Manager, ("Failed to create file for built asset at %s.",
            pathBuilt.c_str()));
        }
      }

      if (!done)  // write header
      {
        AssetHeader header { asset->GetDescriptor().type, version, 0 };
        done = !assetWriter.Write(&header, sizeof(header), 1);
        if (done)
        {
          XR_TRACE(Asset::Manager, ("Failed to write header for built asset at %s.",
            pathBuilt.c_str()));
        }
      }

      if (!done)  // build asset
      {
        done = !iFind->second->Build(assetPath.GetNameExt(), fb.GetData(), fb.GetSize(),
          assetWriter);
        if (done)
        {
          XR_TRACE(Asset::Manager, ("Failed to build asset '%s'.", pathBuilt.c_str()));
        }
      }

      if (done)
      {
        asset->FlagError();
      }
    }
  }
}
#endif

void Asset::Manager::LoadInternal(uint16_t version, FilePath const& path,
  Asset::Ptr const& asset, FlagType flags)
{
#ifdef XR_DEBUG
  // Store the original path for debugging purposes
  asset->m_debugPath = path.c_str();
#endif

  // Clear private flags and set Loading
  asset->OverrideFlags(PrivateMask, LoadingFlag);

#ifdef ENABLE_ASSET_BUILDING
  BuildAsset(version, path, asset);

  // If we're flawless, process job.
  if (!CheckAllMaskBits(asset->GetFlags(), ErrorFlag))
#endif  // ENABLE_ASSET_BUILDING
  {
    LoadAsset(version, asset, flags);
  }
}

//==============================================================================
void Asset::Manager::LoadInternal(uint16_t version, Ptr const& asset, FlagType flags)
{
#ifdef XR_DEBUG
  // Clear the debug path -- the descriptor tells you where the asset is.
  asset->m_debugPath.clear();
#endif

  // Clear private flags and set Loading
  asset->OverrideFlags(PrivateMask, LoadingFlag);

  LoadAsset(version, asset, flags);
}

//==============================================================================
bool Asset::Manager::Remove(Asset& asset)
{
  bool success = !CheckAllMaskBits(asset.GetFlags(), UnmanagedFlag);
  if (success)
  {
    success = s_assetMan.RemoveManaged(asset);
  }
  return success;
}

//==============================================================================
void Asset::Manager::UnloadUnused()
{
  s_assetMan.UnloadUnused();
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

//==============================================================================
bool Asset::Load(size_t size, uint8_t const* buffer)
{
  XR_ASSERTMSG(Asset, !CheckAllMaskBits(m_flags, LoadingFlag),
    ("Loading is already in progress; it's bound to clobber what's being set."));
  bool success = OnLoaded(size, buffer);
  if (success)
  {
    OverrideFlags(Asset::LoadedFlag, Asset::ReadyFlag);
  }
  else
  {
    FlagError();
  }
  return success;
}

//==============================================================================
bool Asset::Unload()
{
  bool  doUnload = false;
  {
    std::unique_lock<Spinlock> lock(m_flaglock);
    doUnload = CheckAllMaskBits(m_flags, ReadyFlag);
    if(doUnload || CheckAllMaskBits(m_flags, ErrorFlag))
    {
      m_flags = m_flags & ~PrivateMask;
    }
  }

  if(doUnload)
  {
    OnUnload();
  }
  return doUnload;
}

} // XR

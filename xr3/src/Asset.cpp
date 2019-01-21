//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Asset.hpp"
#include "xr/FileBuffer.hpp"
#include "xr/FileWriter.hpp"
#include "xr/threading/Worker.hpp"
#include "xr/memory/ScopeGuard.hpp"
#include "xr/Hash.hpp"
#include <map>

#ifdef ENABLE_ASSET_BUILDING
#include <unordered_map>
#endif

#define LTRACE(format) XR_TRACE(Asset::Manager, format)
#define LTRACEIF(condition, format) XR_TRACEIF(Asset::Manager, condition, format)

namespace xr
{
namespace
{

//==============================================================================
struct AssetHeader
{
  Asset::TypeId typeId;
  Asset::VersionType version;
  uint16_t reserved;
};

using NumDependenciesType = uint16_t;
using DependencyPathLenType = uint16_t;

//==============================================================================
const size_t kChunkSizeBytes = XR_KBYTES(16);

class LoadJob : public Worker::Job
{
public:
  // data
  Asset::Ptr asset;

  // structors
  explicit LoadJob(File::Handle hFile, size_t size, Asset::Ptr const& a)
  : asset(a),
    m_hFile(hFile),
    m_data(size)
  {}

  ~LoadJob()
  {
    CloseHandle();
  }

  // general
  virtual void Start()
  {
    m_nextWrite = m_data.data();
  }

  virtual bool Process()
  {
    const auto progress = m_nextWrite - m_data.data();
    const size_t nextChunkSize = std::min(kChunkSizeBytes, m_data.size() - progress);
    const size_t readSize = File::Read(m_hFile, 1, nextChunkSize, m_nextWrite);
    bool done = readSize != nextChunkSize;
    if (done)
    {
      LTRACE(("Failed to read %d bytes of asset '%s' @ %d of %d bytes",
        nextChunkSize, asset->GetDescriptor().ToPath().c_str(), progress,
        m_data.size()));
      asset->FlagError();
    }
    else
    {
      m_nextWrite += readSize;

      done = progress + readSize == m_data.size();
      if (done)
      {
        asset->OverrideFlags(Asset::LoadingFlag, Asset::ProcessingFlag);
      }
    }

    return done;
  }

  bool ProcessData()
  {
    return asset->ProcessData({ m_data.size(), m_data.data() });
  }

private:
  // data
  File::Handle          m_hFile = nullptr;
  std::vector<uint8_t>  m_data;
  uint8_t*              m_nextWrite = nullptr;

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
class AssetManagerImpl // TODO: improve encapsulation of members
{
public:
  // structors
  AssetManagerImpl(FilePath path, Allocator* alloc)
  {
    if(path.StartsWith(File::kRawProto))
    {
      path = path.c_str() + File::kRawProto.size();
    }

    path = File::StripRoots(path);
    if (path.empty())
    {
      path = Asset::Manager::kDefaultPath;
    }
    path.AppendDirSeparator();
    m_path = path;

#ifdef ENABLE_ASSET_BUILDING
    File::MakeDirs(m_path);
#endif

    if (!alloc)
    {
      static Mallocator mallocator;
      alloc = &mallocator;
    }

    m_allocator = alloc;
  }

  ~AssetManagerImpl()
  {
    m_worker.CancelPendingJobs();
    m_worker.Finalize();

    ClearManaged();
  }

  // general
  FilePath const& GetPath()
  {
    return m_path;
  }

  Allocator* GetAllocator()
  {
    return m_allocator;
  }

  bool Manage(Asset::Ptr const& a)
  {
    auto const& desc = a->GetDescriptor();
    bool success = desc.IsValid();
    if (success)
    {
      std::unique_lock<decltype(m_assetsLock)> lock(m_assetsLock);
      auto iFind = m_assets.find(desc);
      success = iFind == m_assets.end();
      if (success)
      {
        m_assets.insert(iFind, { desc, a });
        a->OverrideFlags(Asset::UnmanagedFlag, 0);
      }
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

  void EnqueueJob(LoadJob& lj)
  {
    {
      std::unique_lock<decltype(m_pendingLock)> lock(m_pendingLock);
      m_pending.push_back(&lj);
    }

    m_worker.Enqueue(lj);
  }

  void UpdateJobs()
  {
    decltype(m_pending) q;
    {
      std::unique_lock<decltype(m_pendingLock)> lock(m_pendingLock);
      auto i = m_pending.begin();
      while (i != m_pending.end() &&
        CheckAllMaskBits((*i)->asset->GetFlags(), Asset::ProcessingFlag))
      {
        ++i;
      }

      q.adopt(m_pending, i);
    }

    for (auto i0 = q.begin(), i1 = q.end(); i0 != i1; ++i0)
    {
      auto j = *i0;
      if (!CheckAllMaskBits(j->asset->GetFlags(), Asset::ErrorFlag))
      {
        j->ProcessData();
      }

      DeleteJob(*j);
    }
  }

  void SuspendJobs()
  {
    m_worker.Suspend();
  }

  void ResumeJobs()
  {
    m_worker.Resume();
  }

  void DeleteJob(LoadJob& lj)
  {
    lj.~LoadJob();
    m_allocator->Deallocate(&lj);
  }

  void UnloadUnused()
  {
    std::unique_lock<decltype(m_assetsLock)> lock(m_assetsLock);
    while (UnloadUnusedInternal())
    {}
  }

private:
  // data
  FilePath m_path;
  Allocator* m_allocator;

  Worker m_worker;

  Spinlock m_assetsLock;
  std::map<Asset::DescriptorCore, Asset::Ptr> m_assets;

  Spinlock m_pendingLock;
  Queue<LoadJob*> m_pending;

  // internal
  bool UnloadUnusedInternal()
  {
    bool unloaded = false;
    for (auto& i : m_assets)
    {
      auto& ptr = i.second;
      if (CheckAllMaskBits(ptr->GetFlags(), Asset::ReadyFlag) && ptr->GetRefCount() == 1)
      {
        i.second->Unload();
        unloaded = true;
      }
    }
    return unloaded;
  }

  void ClearManaged()
  {
    std::unique_lock<decltype(m_assetsLock)> lock(m_assetsLock);
    m_assets.clear();
  }
};

//==============================================================================
using ReflectorMap = std::map<Asset::TypeId, detail::AssetReflector const*>;
ReflectorMap s_reflectors;

std::map<uint32_t, ReflectorMap::iterator> s_extensions;

#ifdef ENABLE_ASSET_BUILDING
std::unordered_map<Asset::TypeId, Asset::Builder const*> s_assetBuilders;
#endif

std::unique_ptr<AssetManagerImpl> s_assetMan;

//==============================================================================
void RegisterReflector(detail::AssetReflector const& r)
{
  // Register reflector.
  auto iReflector = s_reflectors.find(r.type);
  XR_ASSERTMSG(Asset::Manager, iReflector == s_reflectors.end(),
    ("Reflector already registered for type '%.*s'.", sizeof(r.type), &r.type));
  iReflector = s_reflectors.insert(iReflector, { r.type, &r });

  // Hash and map extensions to reflector [registration].
  auto exts = r.extensions;
  while (exts)
  {
    auto next = strchr(exts, ';');
    size_t size = next ? next - exts : strlen(exts);
    if (size > 0)
    {
      const uint32_t hash = Hash::String32(exts, size);
      auto iExt = s_extensions.find(hash);
      XR_ASSERTMSG(Asset::Manager, iExt == s_extensions.end(),
        ("Extension %.*s already registered.", size, exts));
      s_extensions.insert(iExt, { hash, iReflector });

      if (next)
      {
        ++next;
      }
    }
    exts = next;
  }
}

//==============================================================================
#ifdef ENABLE_ASSET_BUILDING
void RegisterBuilder(Asset::Builder const& builder)
{
  auto iBuilder = s_assetBuilders.find(builder.type);
  XR_ASSERTMSG(Asset::Manager, iBuilder == s_assetBuilders.end(),
    ("Builder already registered for type '%.*s'", sizeof(builder.type), &builder.type));
  s_assetBuilders.insert(iBuilder, { builder.type, &builder });
}
#endif  // ENABLE_ASSET_BUILDING

//==============================================================================
void LoadAsset(Asset::VersionType version, Asset::Ptr const& asset, Asset::FlagType flags)
{
  FilePath path = Asset::Manager::GetAssetPath() / asset->GetDescriptor().ToPath();
  File::Handle hFile = File::Open(path, "rb");
  bool done = hFile == nullptr;
  if (done)
  {
    LTRACE(("%s: failed to open.", path.c_str()));
  }

  size_t size;
  AssetHeader header;
  if (!done)  // get size
  {
    size = File::GetSize(hFile);
    done = size < sizeof(AssetHeader) ||
      File::Read(hFile, sizeof(header), 1, &header) < 1;
    if (done)
    {
      LTRACE(("%s: failed to read header.", path.c_str()));
    }
    else
    {
      size -= sizeof(header);
    }
  }

  if (!done)  // check typeId
  {
    auto type = asset->GetDescriptor().type;
    done = header.typeId != type;
    if (done)
    {
      LTRACE(("%s: type ID mismatch, expected: %.*s, got: %.*s.", path.c_str(),
        sizeof(type), &type, sizeof(header.typeId), &header.typeId));
    }
  }

  if (!done)  // check version
  {
    done = header.version != version;
    if (done)
    {
      LTRACE(("%s: version mismatch, expected: %d, got: %d", path.c_str(), version,
        header.version));
    }
  }

  if (!done)  // load dependencies
  {
    NumDependenciesType numDependencies;
    done = File::Read(hFile, sizeof(numDependencies), 1, &numDependencies) < 1;
    size -= sizeof(numDependencies);

    NumDependenciesType i = 0;
    DependencyPathLenType len;
    FilePath pathDep;
    while (!done && i < numDependencies)
    {
      done = File::Read(hFile, sizeof(len), 1, &len) < 1 || len > FilePath::kCapacity ||
        len > size || File::Read(hFile, len, 1, pathDep.data()) < 1;
      if (done)
      {
        LTRACE(("%s: failed to read dependency name %d of %d.", path.c_str(), i,
          numDependencies));
      }
      else
      {
        pathDep.data()[len] = '\0';
        pathDep.UpdateSize();

        size -= sizeof(DependencyPathLenType) + len;

        auto dependency = Asset::Manager::LoadReflected(pathDep, flags);
        done = !dependency || CheckAllMaskBits(dependency->GetFlags(), Asset::ErrorFlag);
        ++i;
      }
    }
  }

  if (!done)  // allocate buffer, create and perform / create job.
  {
    if (CheckAllMaskBits(flags, Asset::LoadSyncFlag))
    {
      LoadJob lj(hFile, size, asset);
      lj.Start();
      while (!lj.Process())
      {}

      if (CheckAllMaskBits(lj.asset->GetFlags(), Asset::ProcessingFlag))
      {
        lj.ProcessData();
      }
    }
    else
    {
      void* jobBuffer = s_assetMan->GetAllocator()->Allocate(sizeof(LoadJob));
      auto lj = new (jobBuffer) LoadJob(hFile, size, asset);
      s_assetMan->EnqueueJob(*lj);
    }
  }
  else
  {
    asset->FlagError();
  }
}

#ifdef ENABLE_ASSET_BUILDING
//==============================================================================
void BuildAsset(FilePath const& path, Asset::VersionType version, Asset::Ptr const& asset)
{
  bool forceBuild = CheckAllMaskBits(asset->GetFlags(), Asset::ForceBuildFlag);

  // Check the name -- are we trying to load a raw or a built asset?
  // Strip ram/rom and asset roots.
  FilePath rawPath(File::StripRoots(path));
  if (rawPath.StartsWith(s_assetMan->GetPath()))
  {
    rawPath = FilePath(rawPath.c_str() + s_assetMan->GetPath().size());
  }

  auto desc = asset->GetDescriptor();
  FilePath builtPath = desc.ToPath();
  bool rebuild = builtPath != rawPath;
  if (rebuild || forceBuild) // If we're building, we'll need the correct asset path.
  {
    builtPath = Asset::Manager::GetAssetPath() / builtPath;
  }

  if (rebuild)
  {
    // Check for raw and built asset, compare last modification time.
    auto tsRaw = File::GetModifiedTime(path); // must use original path!
    XR_ASSERTMSG(Asset::Manager, tsRaw > 0, ("'%s' doesn't exist.", path.c_str()));
    auto tsBuilt = File::GetModifiedTime(builtPath.c_str());

    if (tsBuilt > 0 && tsBuilt >= tsRaw) // if built asset exists and isn't older
    {
      auto hFile = File::Open(builtPath, "rb");
      auto guard = MakeScopeGuard([&hFile] {
        File::Close(hFile);
      });

      // Try to see what type / version we've got here. If the file doesn't exist
      // or can't be read, we'll rebuild. Persistent I/O errors will be dealt with
      // later.
      AssetHeader header = { 0, 0 };
      if (File::Read(hFile, sizeof(header), 1, &header) == 1) // asset [header] is readable
      {
        if (header.typeId != desc.type)
        {
          LTRACE(("%s: Hash clash detected trying to build %s, pre-existing type: %.*s.",
            path.c_str(), builtPath.c_str(), sizeof(header.typeId), &header.typeId));
          asset->FlagError();
          rebuild = false;  // hash clash -- error;
        }
        else if (header.version == version)
        {
          rebuild = false;  // version matches -- rebuild not required;
        }
      }
    }
  }

  if (rebuild || forceBuild) // Perform the building of the asset.
  {
    auto iFind = s_assetBuilders.find(desc.type);
    bool done = iFind == s_assetBuilders.end();
    LTRACEIF(done, ("%s: failed to find builder for type '%.*s'.", path.c_str(),
      sizeof(desc.type), &desc.type));

    FileBuffer fb;
    if (!done)  // read source file
    {
      done = !fb.Open(path);
      LTRACEIF(done, ("%s: failed to read source.", path.c_str()));
    }

    if (!done)  // make asset directory if need be
    {
      fb.Close();

      builtPath = File::GetRamPath() / builtPath;
      done = !File::MakeDirs(builtPath);
      LTRACEIF(done, ("%s: failed to create directory structure for '%s'.",
        path.c_str(), builtPath.c_str()));
    }

    auto assetGuard = MakeScopeGuard([&builtPath](){
      if (!File::Delete(builtPath))
      {
        LTRACE(("Failed to remove half-written asset."));
      }
    });
    FileWriter assetWriter; // must be declared after the guard since it'll be cleared up backwards.
    if (!done)  // create asset file
    {
      done = !assetWriter.Open(builtPath, FileWriter::Mode::Truncate, false);
      LTRACEIF(done, ("%s: failed to create file for built asset at '%s'.",
        path.c_str(), builtPath.c_str()));
    }

    if (!done)  // write header
    {
      AssetHeader header{ desc.type, version, 0 };
      done = !assetWriter.Write(&header, sizeof(header), 1);
      LTRACEIF(done, ("%s: failed to write asset header to '%s'.", path.c_str(),
        builtPath.c_str()));
    }

    std::vector<FilePath> dependencies;
    std::ostringstream assetData;
    if (!done)  // build asset
    {
      done = !iFind->second->Build(rawPath.GetNameExt(), { fb.GetSize(), fb.GetData() },
        dependencies, assetData);
      LTRACEIF(done, ("%s: failed to build asset.", path.c_str()));
    }

    if (!done)  // write dependencies
    {
      done = !assetWriter.Write(NumDependenciesType(dependencies.size()));
      for (auto i0 = dependencies.begin(), i1 = dependencies.end(); !done && i0 != i1; ++i0)
      {
        done = !(assetWriter.Write(DependencyPathLenType(i0->size())) &&
          assetWriter.Write(i0->data(), 1, i0->size()));
      }
      LTRACEIF(done, ("%s: failed to write dependencies to %s.", path.c_str(),
        builtPath.c_str()));
    }

    if (!done) // write built asset
    {
      auto str = assetData.str();
      done = !assetWriter.Write(str.data(), 1, str.size());
      LTRACEIF(done, ("%s: failed to write asset data to %s.", path.c_str(),
        builtPath.c_str()));
    }

    if (done)
    {
      asset->FlagError();
    }
    else
    {
      assetGuard.Release();
    }
  }
}
#endif

} // nonamespace

//==============================================================================
char const* const Asset::Manager::kDefaultPath = "assets";

//==============================================================================
void Asset::Manager::Init(FilePath const& path, Allocator* alloc)
{
  s_assetMan.reset(new AssetManagerImpl(path, alloc));

  detail::AssetReflector::Base::ForEach(RegisterReflector);
#ifdef ENABLE_ASSET_BUILDING
  Builder::Base::ForEach(RegisterBuilder);
#endif
}

//==============================================================================
const FilePath& Asset::Manager::GetAssetPath()
{
  return s_assetMan->GetPath();
}

//==============================================================================
Asset::HashType Asset::Manager::HashPath(FilePath path)
{
  path = File::StripRoots(path);
  // Strip the asset path too, as the user is not required to specify it to
  // operate on assets.
  if (path.StartsWith(s_assetMan->GetPath()))
  {
    path = FilePath(path.c_str() + s_assetMan->GetPath().size());
  }

  // We have a built asset if the remaining path is in our built asset path format:
  // - no extension
  // - 16 digits hex name
  // - in a two-deep directory structure, the parent directories being the first
  //   and second characters of the [hashed] name.
  char const* nameExt = path.GetNameExt();
  Asset::HashType hash;
  if (!path.GetExt() &&
    path.size() == 20 &&
    nameExt == path.data() + 4 &&
    sscanf(nameExt, "%" PRIx64, &hash) == 1)
  {
    if (DescriptorCore(0, hash).ToPath() != path)
    {
      hash = 0;
    }
  }
  else
  {
    // Otherwise hash what's left, which must not be an empty string.
    const size_t size = path.size();
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
Asset::Ptr Asset::Manager::LoadReflected(FilePath const& path, FlagType flags)
{
  flags &= ~PrivateMask;
  DescriptorCore  desc(0, HashPath(path));

  auto ext = path.GetExt();
  if (ext)
  {
    uint32_t hash = Hash::String32(ext);
    auto iFind = s_extensions.find(hash);
    if (iFind != s_extensions.end())
    {
      desc.type = iFind->second->first;
    }
  }

  Ptr asset;
  if (desc.HasType())
  {
    VersionType version;
    asset = FindOrReflectorCreate(desc, flags, version);

    auto foundFlags = asset->GetFlags();
    if (IsLoadable(foundFlags, flags))
    {
      LoadInternal(version, path, asset, flags);
    }
  }

  return asset;
}

//==============================================================================
Asset::Ptr Asset::Manager::LoadReflected(DescriptorCore const& desc, FlagType flags)
{
  flags &= ~PrivateMask;

  Ptr asset;
  if (desc.HasType())
  {
    VersionType version;
    asset = FindOrReflectorCreate(desc, flags, version);

    auto foundFlags = asset->GetFlags();
    if (IsLoadable(foundFlags, flags))
    {
      LoadInternal(version, asset, flags);
    }
  }

  return asset;
}

//==============================================================================
Asset::Ptr Asset::Manager::Find(DescriptorCore const& desc)
{
  return s_assetMan->FindManaged(desc);
}

//==============================================================================
bool Asset::Manager::Manage(Asset::Ptr const& asset)
{
  XR_ASSERT(Asset::Manager, asset);
  bool success = s_assetMan->Manage(asset);
  return success;
}

//==============================================================================
Asset::Ptr Asset::Manager::FindOrReflectorCreate(DescriptorCore const& desc,
  FlagType flags, VersionType& outVersion)
{
  Asset::Ptr asset;
  if (!CheckAllMaskBits(flags, UnmanagedFlag))
  {
    asset = Find(desc);
  }

  auto iFind = s_reflectors.find(desc.type);
  XR_ASSERT(Asset::Manager, iFind != s_reflectors.end());

  auto reflector = iFind->second;
  XR_ASSERT(Asset::Manager, reflector);

  outVersion = reflector->version;

  if (!asset)
  {
    asset.Reset((*reflector->create)(desc.hash, flags));
    if (!CheckAllMaskBits(flags, Asset::UnmanagedFlag))
    {
      Asset::Manager::Manage(asset);
    }
  }
  return asset;
}

//==============================================================================
bool Asset::Manager::IsLoadable(FlagType oldFlags, FlagType newFlags)
{
  return !(CheckAllMaskBits(oldFlags, LoadingFlag) ||
    (CheckAnyMaskBits(oldFlags, ProcessingFlag | ReadyFlag) &&
      !CheckAllMaskBits(newFlags, ForceReloadFlag)));
}

//==============================================================================
void Asset::Manager::LoadInternal(VersionType version, FilePath const& path,
  Asset::Ptr const& asset, FlagType flags)
{
#ifdef XR_DEBUG
  // Store the original path for debugging purposes
  asset->m_debugPath = path.c_str();
#endif

  // Clear private flags and set Loading
  asset->OverrideFlags(PrivateMask, LoadingFlag);

#ifdef ENABLE_ASSET_BUILDING
  BuildAsset(path, version, asset);

  // If we're flawless, process job.
  if (!CheckAllMaskBits(asset->GetFlags(), ErrorFlag))
#endif  // ENABLE_ASSET_BUILDING
  {
    LoadAsset(version, asset, flags);
  }
}

//==============================================================================
void Asset::Manager::LoadInternal(VersionType version, Ptr const& asset, FlagType flags)
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
    success = s_assetMan->RemoveManaged(asset);
  }
  return success;
}

//==============================================================================
void Asset::Manager::UnloadUnused()
{
  s_assetMan->UnloadUnused();
}

//==============================================================================
void Asset::Manager::Update()
{
  s_assetMan->UpdateJobs();
}

//==============================================================================
void Asset::Manager::Suspend()
{
  s_assetMan->SuspendJobs();
}

//==============================================================================
void Asset::Manager::Resume()
{
  s_assetMan->ResumeJobs();
}

//==============================================================================
void Asset::Manager::Shutdown()
{
#ifdef ENABLE_ASSET_BUILDING
  s_assetBuilders.clear();
#endif
  s_extensions.clear();
  s_reflectors.clear();
  s_assetMan.reset(nullptr);
}

//==============================================================================
Asset* Asset::Reflect(TypeId typeId, HashType hash, FlagType flags)
{
  auto iFind = s_reflectors.find(typeId);
  XR_ASSERT(Asset, iFind != s_reflectors.end());
  return (*iFind->second->create)(hash, flags);
}

//==============================================================================
bool Asset::ProcessData(Buffer const& buffer)
{
  OnUnload();

  XR_ASSERTMSG(Asset, !CheckAllMaskBits(m_flags, LoadingFlag),
    ("Loading is in progress; it's bound to clobber the flags being set."));
  bool success = OnLoaded(buffer);
  if (success)
  {
    OverrideFlags(Asset::ProcessingFlag, Asset::ReadyFlag);
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
    XR_ASSERTMSG(Asset, !CheckAllMaskBits(m_flags, LoadingFlag),
      ("Unload when loading is in progress will lead to unexpected results."));
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

namespace detail
{

//==============================================================================
AssetReflector const* AssetReflector::GetReflector(const char* extension)
{
  return GetReflector(Hash::String32(extension));
}

//==============================================================================
AssetReflector const* AssetReflector::GetReflector(uint32_t extensionHash)
{
  auto iFind = s_extensions.find(extensionHash);
  return iFind != s_extensions.end() ? iFind->second->second : nullptr;
}

//==============================================================================
xr::detail::AssetReflector::AssetReflector(Asset::TypeId type_,
  Asset::VersionType version_, CreateFn create_, char const* extensions_)
: Linked<AssetReflector const>(*this),
  type(type_),
  version(version_),
  create(create_),
  extensions(extensions_)
{}

} // detail
} // xr

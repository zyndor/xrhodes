//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "AssetLoadJob.hpp"
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
class AssetManagerImpl // TODO: improve encapsulation of members
{
public:
  // structors
  AssetManagerImpl(FilePath const& path, Allocator* alloc)
  : m_path(path)
  {
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

  void EnqueueJob(AssetLoadJob& lj)
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

  void DeleteJob(AssetLoadJob& lj)
  {
    lj.~AssetLoadJob();
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
  Queue<AssetLoadJob*> m_pending;

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

#ifdef ENABLE_ASSET_BUILDING
//==============================================================================
void RegisterBuilder(Asset::Builder const& builder)
{
  auto iBuilder = s_assetBuilders.find(builder.type);
  XR_ASSERTMSG(Asset::Manager, iBuilder == s_assetBuilders.end(),
    ("Builder already registered for type '%.*s'", sizeof(builder.type), &builder.type));
  s_assetBuilders.insert(iBuilder, { builder.type, &builder });
}

//==============================================================================
bool BuildAsset(FilePath const& path, Asset::VersionType version,
  Asset::DescriptorCore const& desc, Asset::FlagType flags)
{
  bool forceBuild = CheckAllMaskBits(flags, Asset::ForceBuildFlag);

  // Check the name -- are we trying to load a raw or a built asset?
  // Strip ram/rom and asset roots.
  FilePath rawPath(File::StripRoots(path));
  if (rawPath.StartsWith(s_assetMan->GetPath()))
  {
    rawPath = FilePath(rawPath.c_str() + s_assetMan->GetPath().size());
  }

  FilePath builtPath = desc.ToPath();
  bool rebuild = builtPath != rawPath;
  if (rebuild || forceBuild) // If we're building, we'll need the correct asset path.
  {
    builtPath = Asset::Manager::GetAssetPath() / builtPath;
  }

  // Pure path is the actual location, without any options.
  XR_ASSERT(Asset, path.GetExt());
  auto iOptions = (path.GetExt() - path.c_str()) + strcspn(path.GetExt(), Asset::kOptionDelimiter);
  auto purePath = FilePath(path.c_str(), iOptions);
  if (rebuild)
  {
    // Check for raw and built asset, compare last modification time.
    auto tsRaw = File::GetModifiedTime(purePath); // must use original path!
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
          return false;
        }
        else if (header.version == version)
        {
          rebuild = false;  // version matches -- rebuild not required;
        }
      }
    }
  }

  if (!(rebuild || forceBuild))
  {
    return true;
  }

  auto iFind = s_assetBuilders.find(desc.type);
  if (iFind == s_assetBuilders.end())
  {
    LTRACE(("%s: failed to find builder for type '%.*s'.", path.c_str(),
      sizeof(desc.type), &desc.type));
    return false;
  }

  FileBuffer srcBuf;
  if (!srcBuf.Open(purePath))  // read source file
  {
    LTRACE(("%s: failed to read source.", path.c_str()));
    return false;
  }

  srcBuf.Close();

  builtPath = File::GetRamPath() / builtPath;
  if (!File::MakeDirs(builtPath))
  {
    LTRACE(("%s: failed to create directory structure for '%s'.", path.c_str(),
      builtPath.c_str()));
    return false;
  }

  auto assetGuard = MakeScopeGuard([&builtPath](){
    if (!File::Delete(builtPath))
    {
      LTRACE(("Failed to remove half-written asset at '%s'.", builtPath.c_str()));
    }
  });
  FileWriter assetWriter; // must be declared after the guard since it'll be cleared up backwards.
  if (!assetWriter.Open(builtPath, FileWriter::Mode::Truncate, false))
  {
    LTRACE(("%s: failed to create file for built asset at '%s'.", path.c_str(),
      builtPath.c_str()));
    return false;
  }

  AssetHeader header{ desc.type, version, 0 };
  if (!assetWriter.Write(&header, sizeof(header), 1))
  {
    LTRACE(("%s: failed to write asset header to '%s'.", path.c_str(),
      builtPath.c_str()));
    return false;
  }

  std::vector<FilePath> dependencies;
  std::ostringstream assetData;
  if (!iFind->second->Build(rawPath.GetNameExt(), { srcBuf.GetSize(), srcBuf.GetData() },
      dependencies, assetData))
  {
    LTRACE(("%s: failed to build asset.", path.c_str()));
    return false;
  }

  if (!assetWriter.Write(NumDependenciesType(dependencies.size())))
  {
    LTRACE(("%s: failed to write number of dependencies to %s.", path.c_str(),
      builtPath.c_str()));
    return false;
  }

  for (auto i0 = dependencies.begin(), i1 = dependencies.end(); i0 != i1; ++i0)
  {
    if (!(assetWriter.Write(DependencyPathLenType(i0->size())) &&
      assetWriter.Write(i0->data(), 1, i0->size())))
    {
      LTRACE(("%s: failed to write dependencies to %s.", path.c_str(),
        builtPath.c_str()));
      return false;
    }
  }

  auto str = assetData.str();
  if (!assetWriter.Write(str.data(), 1, str.size()))
  {
    LTRACE(("%s: failed to write asset data to %s.", path.c_str(),
      builtPath.c_str()));
    return false;
  }

  assetGuard.Release();
  return true;
}
#endif  // ENABLE_ASSET_BUILDING

//==============================================================================
bool IsAssetLoadable(Asset::FlagType oldFlags, Asset::FlagType newFlags)
{
  // TODO: handle ErrorFlag and ForceRebuildFlag
  return !(CheckAllMaskBits(oldFlags, Asset::LoadingFlag) ||
    (CheckAnyMaskBits(oldFlags, Asset::ProcessingFlag | Asset::ReadyFlag) &&
      !CheckAnyMaskBits(newFlags, Asset::ForceReloadFlag)));
}

//==============================================================================
void LoadAsset(Asset::VersionType version, Asset& asset, Asset::FlagType flags)
{
  asset.OverrideFlags(Asset::PrivateMask, Asset::LoadingFlag);

  auto errorGuard = MakeScopeGuard([&asset]() {
    asset.FlagError();
  });

  FilePath path = Asset::Manager::GetAssetPath() / asset.GetDescriptor().ToPath();
  File::Handle hFile = File::Open(path, "rb");
  if (hFile == nullptr)
  {
    LTRACE(("%s: failed to open.", path.c_str()));
    return;
  }

  AssetHeader header;
  size_t size = File::GetSize(hFile);
  if (size < sizeof(AssetHeader) ||
    File::Read(hFile, sizeof(header), 1, &header) < 1)
  {
    LTRACE(("%s: failed to read header.", path.c_str()));
    return;
  }

  size -= sizeof(header);
  auto type = asset.GetDescriptor().type;
  if (header.typeId != type)
  {
    LTRACE(("%s: type ID mismatch, expected: %.*s, got: %.*s.", path.c_str(),
      sizeof(type), &type, sizeof(header.typeId), &header.typeId));
    return;
  }

  if (header.version != version)
  {
    LTRACE(("%s: version mismatch, expected: %d, got: %d", path.c_str(), version,
      header.version));
    return;
  }

  NumDependenciesType numDependencies;
  if (File::Read(hFile, sizeof(numDependencies), 1, &numDependencies) < 1)
  {
    LTRACE(("%s: failed to read number of dependencies.", path.c_str()));
    return;
  }

  size -= sizeof(numDependencies);

  NumDependenciesType i = 0;
  DependencyPathLenType len;
  FilePath pathDep;
  while (i < numDependencies)
  {
    if (File::Read(hFile, sizeof(len), 1, &len) < 1 || len > FilePath::kCapacity ||
      len > size || File::Read(hFile, len, 1, pathDep.data()) < 1)
    {
      LTRACE(("%s: failed to read dependency name %d of %d.", path.c_str(), i,
        numDependencies));
      return;
    }
    else
    {
      pathDep.data()[len] = '\0';
      pathDep.UpdateSize();

      size -= sizeof(DependencyPathLenType) + len;

      auto dependency = Asset::Manager::LoadReflected(pathDep, flags);
      if (!dependency || CheckAllMaskBits(dependency->GetFlags(), Asset::ErrorFlag))
      {
        LTRACE(("%s: error loading dependency '%s'.", path.c_str(), pathDep.c_str()));
        return;
      }
      ++i;
    }
  }

  if (CheckAllMaskBits(flags, Asset::LoadSyncFlag))
  {
    AssetLoadJob lj(hFile, size, Asset::Ptr(&asset));
    lj.Start();
    while (!lj.Process())
    {
    }

    if (CheckAllMaskBits(lj.asset->GetFlags(), Asset::ProcessingFlag))
    {
      lj.ProcessData();
    }
  }
  else
  {
    void* jobBuffer = s_assetMan->GetAllocator()->Allocate(sizeof(AssetLoadJob));
    auto lj = new (jobBuffer) AssetLoadJob(hFile, size, Asset::Ptr(&asset));
    s_assetMan->EnqueueJob(*lj);
  }

  errorGuard.Release();
}

} // nonamespace

//==============================================================================
Asset::Builder::Builder(TypeId type_)
: Base(*this),
  type(type_)
{}

//==============================================================================
Asset::Builder::~Builder() = default;

//==============================================================================
char const* const Asset::Manager::kDefaultPath = "assets";

//==============================================================================
void Asset::Manager::Init(FilePath path, Allocator* alloc)
{
  if (path.StartsWith(File::kRawProto))
  {
    path = path.c_str() + File::kRawProto.size();
  }

  path = File::StripRoots(path);
  if (path.empty())
  {
    path = Asset::Manager::kDefaultPath;
  }
  path.AppendDirSeparator();

#ifdef ENABLE_ASSET_BUILDING
  File::MakeDirs(path);
#endif

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
      hash = Hash::String(path.c_str(), size);
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
    // If there is an extension, i.e. the asset isn't built, then we'd like
    // it without any options, thanks.
    auto extLen = strcspn(ext, kOptionDelimiter);
    uint32_t hash = Hash::String32(ext, extLen);
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
    if (IsAssetLoadable(foundFlags, flags))
    {
      LoadInternal(version, path, *asset, flags);
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
    if (IsAssetLoadable(foundFlags, flags))
    {
      LoadInternal(version, *asset, flags);
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
    asset.Reset(reflector->create(desc.hash, flags));
    if (!CheckAllMaskBits(flags, Asset::UnmanagedFlag))
    {
      Asset::Manager::Manage(asset);
    }
  }
  return asset;
}

//==============================================================================
void Asset::Manager::LoadInternal(VersionType version, FilePath const& path,
  Asset& asset, FlagType flags)
{
#ifdef ENABLE_ASSET_BUILDING
  if (!BuildAsset(path, version, asset.GetDescriptor(), flags))
  {
    return;
  }
#endif  // ENABLE_ASSET_BUILDING

#ifdef XR_DEBUG
  // Store the original path for debugging purposes
  asset.m_debugPath = path.c_str();
#endif

  if (IsAssetLoadable(asset.GetFlags(), flags))
  {
    LoadAsset(version, asset, flags);
  }
}

//==============================================================================
void Asset::Manager::LoadInternal(VersionType version, Asset& asset, FlagType flags)
{
#ifdef XR_DEBUG
  // Clear the debug path -- the descriptor tells you where the asset is.
  asset.m_debugPath.clear();
#endif

  if (IsAssetLoadable(asset.GetFlags(), flags))
  {
    LoadAsset(version, asset, flags);
  }
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
const char* const Asset::kOptionDelimiter = "$";

Asset* Asset::Reflect(TypeId typeId, HashType hash, FlagType flags)
{
  auto iFind = s_reflectors.find(typeId);
  XR_ASSERT(Asset, iFind != s_reflectors.end());
  return iFind->second->create(hash, flags);
}

//==============================================================================
Asset::~Asset() = default;

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

//==============================================================================
Asset::Asset(DescriptorCore const& desc, FlagType flags)
: m_descriptor(desc),
  m_flags(flags & ~PrivateMask),
  m_refs()
{}

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
AssetReflector::AssetReflector(Asset::TypeId type_, Asset::VersionType version_,
  CreateFn create_, char const* extensions_)
: Linked<AssetReflector const>(*this),
  type(type_),
  version(version_),
  create(create_),
  extensions(extensions_)
{}

} // detail
} // xr

#ifndef XR_ASSET_HPP
#define XR_ASSET_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/File.hpp"
#include "xr/Hash.hpp"
#include "xr/memory/Counted.hpp"
#include "xr/threading/Counter.hpp"
#include "xr/memory/Buffer.hpp"
#include "xr/memory/memory.hpp"
#include "xr/types/Linked.hpp"
#include "xr/types/fundamentals.hpp"
#include "xr/utils.hpp"
#include "xr/debug.hpp"
#include <memory>
#include <vector>
#include <ostream>
#include <cinttypes>

namespace xr
{

class FileWriter;

//==============================================================================
///@brief The Asset API provides facilities to create and asynchronously load
/// assets, while managing dependencies and ownership.
///@par The Asset::Manager builds assets from their raw format upon loading, in
/// ENABLE_ASSET_BUILDING versions of XRhodes (as Debug is), if the raw asset is
/// newer than its built counterpart, or the asset version was mismatched, or if
/// the ForceBuild flag was specified.
///@par The location that the built assets are saved is ${ram path}/${asset path}
/// (refer to xr::File for more information).
///@par Built assets are identified by a hash of their original (relative) path.
///@par The Asset system supports 'asset options' which may be supplied at load
/// time (be it procedural or via asset dependencies), by adding identifiers to
/// the name of the asset prefixed with Asset::kOptionDelimiter, i.e.
/// path/to/my.asset$option1$option2. Asset options therefore form part of the
/// hash of the built asset.
///@note Use XR_ASSET_DECL and XR_ASSET_DEF to facilitate implementation of
/// concrete Asset types.
class Asset: public Countable
{
public:
  // types
  using Ptr = Counted<Asset>;
  using TypeId = uint32_t;
  using HashType = uint64_t;
  using VersionType = uint16_t;

  using FlagType = uint32_t;
  enum Flags: FlagType
  {
    // private - cannot be set by client code, at the point of loading an Asset.
    ///@brief The processing of the asset stops when this is set. Note: error
    /// states are odd numbers.
    ErrorFlag = XR_MASK_ID(FlagType, 0),

    ///@brief Asset data is being loaded from disk.
    LoadingFlag = XR_MASK_ID(FlagType, 1),

    ///@brief Transient state between loading data and completion, to inform Asset::Manager,
    /// and to help tell loading (I/O) and processing errors apart.
    ProcessingFlag = XR_MASK_ID(FlagType, 2),

    ///@brief The Asset is ready to use.
    ReadyFlag = XR_MASK_ID(FlagType, 3),

    // public
    ///@brief Load() will not load an already loaded asset, unless this flag is set.
    ForceReloadFlag = XR_MASK_ID(FlagType, 4),

    ///@brief Forces the asset to load synchronously.
    LoadSyncFlag = XR_MASK_ID(FlagType, 5),

    ///@brief The asset manager will not keep a reference to the Asset, Find()ing
    /// this Asset will fail.
    UnmanagedFlag = XR_MASK_ID(FlagType, 6),

    ///@brief Signifies a request to keep the source data, which is discarded by
    /// default. Handling this flag is a responsibility of the concrete Asset type.
    KeepSourceDataFlag = XR_MASK_ID(FlagType, 7),

    // ENABLE_ASSET_BUILDING-only flags. Intended for debug / testing.
    ///@brief Forces the building of the asset.
    ForceBuildFlag = XR_MASK_ID(FlagType, 8),

    ///@brief This signifies a request for suppressing calls to subsystems (e.g.
    /// graphics) when processing assets. Handling this flag is a responsibility
    /// of the concrete Asset type.
    DryRunFlag = XR_MASK_ID(FlagType, 9),

    ///@brief This is the first flag that client code may declare.
    FirstUserFlag = XR_MASK_ID(FlagType, 16),

    PrivateMask = ErrorFlag | ReadyFlag | ProcessingFlag | LoadingFlag
  };

  ///@brief Carries information about the type and identifier of an Asset.
  struct DescriptorCore
  {
    explicit DescriptorCore(TypeId type_ = 0, HashType hash_ = 0)
    : type(type_),
      hash(hash_)
    {}

    bool HasType() const
    {
      return type != 0;
    }

    bool IsValid() const
    {
      return HasType() && hash != 0;
    }

    FilePath ToPath() const
    {
      char hashBuffer[17];
      sprintf(hashBuffer, "%.16" PRIx64, hash);
      FilePath path;
      path.append(hashBuffer, 1);
      path.AppendDirSeparator();
      path.append(hashBuffer + 1, 1);
      path.AppendDirSeparator();
      path += hashBuffer;
      return path;
    }

    bool operator<(DescriptorCore const& rhs) const
    {
      return hash < rhs.hash || (hash == rhs.hash && type < rhs.type);
    }

    bool operator==(DescriptorCore const& rhs) const
    {
      return type == rhs.type && hash == rhs.hash;
    }

    TypeId type;
    HashType hash;
  };

  ///@brief Descriptor of a given type of Asset.
  template <class T>
  struct Descriptor final: DescriptorCore
  {
    using Type = T;

    explicit Descriptor(HashType hash_ = 0)
    : DescriptorCore(Type::kTypeId, hash_)
    {}
  };

  ///@brief Builders are responsible for converting raw assets into engine
  /// format. When asset building is enabled, assets that are loaded by their
  /// raw path are eligible for checking against their built counterpart, and
  /// if change is detected in modification date or asset version, the Manager
  /// will invoke the Builder based on the type of the asset being loaded.
  class Builder: Linked<Builder const>
  {
  public:
    // types
    using Base = Linked<Builder const>;

    // data
    const TypeId type;

    // structors
    explicit Builder(TypeId type_)
    : Base(*this),
      type(type_)
    {}

    virtual ~Builder()
    {}

    ///@brief Parses @a buffer of size @a size bytes, and produces two buffers
    /// of @a dependencies and asset @a data for writing into the built asset.
    /// @a data is what's passed to Asset::ProcessData() when loading; @a dependencies
    /// are paths to assets which will be loaded by the Manager prior to the
    /// loading of the given asset.
    virtual bool Build(char const* rawNameExt, Buffer buffer,
      std::vector<FilePath>& dependencies, std::ostream& data) const = 0;
  };

  ///@brief Offers synchronous and asynchronous loading, and maintains a map,
  /// of assets.
  struct Manager
  {
    static char const* const kDefaultPath;

    ///@brief Initialises the Asset::Manager with the given asset path, which
    /// is a location that may exist in both of the RAM and ROM paths of the
    /// filesystem (if RAM or ROM was specified, it will be stripped), where
    /// built assets are located.
    ///@note It cannot be a raw:// path, nor an explicit RAM or ROM path; these
    /// will be stripped from the supplied path.
    ///@note The asset path should not contain any other data, at the risk of
    /// being overwrittens.
    static void Init(FilePath const& path = kDefaultPath, Allocator* alloc = nullptr);

    static const FilePath& GetAssetPath();

    ///@brief Calculates the hash of the given path.
    ///@note If the file is in the asset path (given to Init()), then it
    /// must be a valid built asset.
    ///@note Raw assets' hash is calculated from their location, after the
    /// stripping of ram / rom and asset paths (but not the rest of the path).
    static HashType HashPath(FilePath path);

    ///@brief Attempts to load a built asset from the asset path that the given
    /// @a path hashes to. If asset building is enabled, the actual @a path will
    /// be checked for a raw asset either more recently modified than its built
    /// counterpart, or having a different version, and the asset will be rebuilt.
    /// This must succeed for the successful loading of the asset.
    template <class T>
    static Counted<T> Load(FilePath const& path, FlagType flags = 0)
    {
      flags &= ~PrivateMask;
      Descriptor<T> desc(HashPath(path));
      Counted<T> asset = FindOrCreateInternal<T>(desc, flags);

      auto foundFlags = asset->GetFlags();
      if (IsLoadable(foundFlags, flags))
      {
        LoadInternal(T::kVersion, path, asset.template StaticCast<Asset>(), flags);
      }
      return asset;
    }

    ///@brief Attempts to load a built asset from given descriptor, either
    /// asynchronously (Manager::Update() will need to be called regularly),
    /// or synchronously (if the LoadSyncFlag was set).
    template <class T>
    static Counted<T> Load(Descriptor<T> const& desc, FlagType flags = 0)
    {
      flags &= ~PrivateMask;
      Counted<T> asset = FindOrCreateInternal<T>(desc, flags);

      auto foundFlags = asset->GetFlags();
      if (IsLoadable(foundFlags, flags))
      {
        LoadInternal(T::kVersion, asset.template StaticCast<Asset>(), flags);
      }
      return asset;
    }

    ///@brief Attempts to load a built asset of unknown type from the asset path
    /// that the given @a path hashes to. If asset building is enabled, the actual
    /// @a path will be checked for a raw asset either more recently modified than
    /// its built counterpart, or having a different version, and the asset will
    /// be rebuilt. This must succeed for the successful loading of the asset.
    static Ptr LoadReflected(FilePath const& path, FlagType flags = 0);

    ///@brief Attempts to load an asset of unknown type from given descriptor.
    static Ptr LoadReflected(DescriptorCore const& desc, FlagType flags = 0);

    ///@brief Attempts to retrieve an asset of the given descriptor, from the
    /// map of managed assets.
    static Ptr Find(DescriptorCore const& desc);

    ///@brief Attempts to retrieve an asset of the given descriptor, from the
    /// map of managed assets.
    template <class T>
    static Counted<T> Find(Descriptor<T> const& desc);

    ///@brief Attempts to retrieve an asset that the given @a path hashes to,
    /// from the map of managed assets.
    template <class T>
    static Counted<T> Find(FilePath const& path);

    ///@brief Attempts to retrieve a managed asset (unless opted out) or
    /// create one (and manage it - unless opted out).
    template <class T>
    static Counted<T> FindOrCreate(DescriptorCore desc, FlagType flags = 0);

    ///@brief Attempts to retrieve a managed asset (unless opted out) or
    /// create one (and manage it - unless opted out).
    template <class T>
    static Counted<T> FindOrCreate(Descriptor<T> const& desc, FlagType flags = 0);

    ///@brief If @a asset wasn't managed, it'll attempt to add it to the map
    /// and clears the UnmanagedFlag on it.
    ///@return The success of the operation, i.e. if the descriptor of @a asset
    /// IsValid(), and was the first amongst the managed assets.
    static bool Manage(Ptr const& asset);

    ///@brief Convenience method to manage a Counted<> of a concrete asset type.
    /// If @a asset wasn't managed, it'll add it to the map and clears the
    /// UnmanagedFlag on it.
    ///@return The success of the operation, i.e. if the descriptor of @a asset
    /// IsValid(), and was the first amongst the managed assets.
    template <class T>
    static bool Manage(Counted<T> const& asset)
    {
      return Manage(Ptr(&*asset));
    }

    ///@brief If @a asset was managed, it'll remove it from the map and sets
    /// UnmanagedFlag on it.
    ///@return The success of the removal, i.e. if it was found in the map.
    static bool Remove(Asset& asset);

    ///@brief Calls Unload() on all assets who have a single reference, held by
    /// the Asset::Manager.
    ///@note This doesn't remove them from the manager.
    static void UnloadUnused();

    ///@brief Pumps the asynchronous asset loading queue, processing the loaded
    /// assets and calling OnLoaded() on the ones that were successful.
    static void Update();

    ///@brief Suspends the [asynchronous] loading of assets.
    static void Suspend();

    ///@brief Resumes the [asynchronous] loading of assets.
    static void Resume();

    static void Shutdown();

  private:
    // internal
    template <class T>
    static Counted<T> CreateInternal(DescriptorCore const& desc, FlagType flags);

    template <class T>
    static Counted<T> FindOrCreateInternal(DescriptorCore const& desc, FlagType flags);

    static Ptr FindOrReflectorCreate(DescriptorCore const& desc, FlagType flags,
      VersionType& outVersion);

    static bool IsLoadable(FlagType oldFlags, FlagType newFlags);

    static void LoadInternal(VersionType version, FilePath const& path, Ptr const& asset,
      FlagType flags);
    static void LoadInternal(VersionType version, Ptr const& asset, FlagType flags);
  };

  // static
  ///@brief Indicates asset options that may be passed as part of the path,
  /// into asset loading functions, e.g. name.ext$option1$option2.
  /// What options are supported, depends on the concrete Asset type.
  static const char* const kOptionDelimiter;

  ///@brief Attempts to create an Asset based on the given @a typeId, and with
  /// the given @a hash and @a flags.
  ///@return The newly created Asset instance.
  static Asset* Reflect(TypeId typeId, HashType hash, FlagType flags = 0);

  // structors
  virtual ~Asset()
  {}

  // general
  DescriptorCore const& GetDescriptor() const
  {
    return m_descriptor;
  }

  FlagType GetFlags() const
  {
    std::unique_lock<Spinlock> lock(m_flaglock);
    return m_flags;
  }

  ///@brief Sets the flags to the given value.
  void OverrideFlags(FlagType flags)
  {
    std::unique_lock<Spinlock> lock(m_flaglock);
    m_flags = flags;
  }

  ///@brief Clears and sets the given bit patterns, in this order.
  void OverrideFlags(FlagType clear, FlagType set)
  {
    std::unique_lock<Spinlock> lock(m_flaglock);
    m_flags = (m_flags & ~clear) | set;
  }

  ///@brief Convenience method to set an error state.
  ///@note It doesn't clear the rest of the [private] flags since those carry
  /// information for the handling of the error.
  void FlagError()
  {
    OverrideFlags(0, ErrorFlag);
  }

  ///@return Whether the Asset can be downcast to type T.
  template <class T>
  bool Is() const
  {
    return m_descriptor.type == T::kTypeId;
  }

  ///@return Pointer to the asset as type T, if it can be downcast, or nullptr
  /// if it cannot.
  template <class T>
  T* Cast()
  {
    return Is<T>() ? static_cast<T*>(this) : nullptr;
  }

  ///@return Pointer-to-a-const to the asset as type T, if it can be downcast,
  /// or nullptr if it cannot.
  template <class T>
  T const* Cast() const
  {
    return Is<T>() ? static_cast<T const*>(this) : nullptr;
  }

  ///@brief Marks a usage or dependency.
  void Acquire()
  {
    m_refs.Acquire();
  }

  ///@return The number of references there are to this Asset.
  int GetRefCount() const
  {
    return m_refs.GetCount();
  }

  ///@brief Marks end of usage / dependency. If this was the last reference
  /// to the asset and it was successfully loaded (ReadyFlag set), then Unload()
  /// will be called on it.
  bool Release()
  {
    bool expired;
    m_refs.Release(expired);
    return expired;
  }

  ///@brief Unloads the asset, then attempts to process the source data in
  /// @a buffer. Will set ReadyFlag it it was successful, and ErrorFlag otherwise.
  ///@note Loading must not be in progress when this is called.
  ///@return The success of the operation.
  bool ProcessData(Buffer const& buffer);

  ///@brief Unloads the Asset (calling OnUnload()) only if it has been loaded.
  /// If it has, or if the ErrorFlag was set, the private flags are cleared.
  ///@return Whether unload has been performed.
  ///@note Loading must not be in progress when this is called.
  ///@note Should be called on the same thread which processing is done on;
  /// this is just a recommendation which the implementation doesn't assert.
  ///@note This doesn't invalidate the Asset object; it may be reloaded based
  /// on its descriptor.
  bool Unload();

protected:
  // types
  using Lock = Counter<Spinlocked>;

  // structors
  explicit Asset(DescriptorCore const& desc, FlagType flags = 0)
  : m_descriptor(desc),
    m_flags(flags & ~PrivateMask),
    m_refs()
  {}

  // data
  DescriptorCore m_descriptor;

  mutable Spinlock m_flaglock;
  FlagType m_flags;

  Lock m_refs;

#ifdef XR_DEBUG
  std::string m_debugPath;
#endif

  // virtual
  ///@brief Performs the actual processing of the data. Called when asset data is
  /// ready to process -- by ProcessData().
  ///@return The success of the operation.
  virtual bool OnLoaded(Buffer buffer) = 0;

  ///@brief Called when unloading of Asset data is requested -- by Unload().
  virtual void OnUnload() = 0;
};

namespace detail
{

class AssetReflector: Linked<AssetReflector const>
{
public:
  // types
  using Base = Linked<AssetReflector const>;
  using CreateFn = Asset*(*)(Asset::HashType, Asset::FlagType);

  // static
  static AssetReflector const* GetReflector(const char* extension);
  static AssetReflector const* GetReflector(uint32_t extensionHash);

  // structors
  AssetReflector(Asset::TypeId type_, Asset::VersionType version_,
    CreateFn create_, char const* extensions_);

  Asset::TypeId type;
  Asset::VersionType version;
  CreateFn create;
  char const* extensions;
};

} // detail

//==============================================================================
// inline
//==============================================================================
template<class T>
inline
Counted<T> Asset::Manager::Find(Descriptor<T> const& desc)
{
  Ptr res = Find(static_cast<DescriptorCore const&>(desc));
  return Counted<T>(res ? res->Cast<T>() : nullptr);
}

//==============================================================================
template<class T>
inline
Counted<T> Asset::Manager::Find(FilePath const& path)
{
  Descriptor<T> desc(HashPath(path));
  return Find(desc);
}

//==============================================================================
template<class T>
inline
Counted<T> Asset::Manager::FindOrCreate(DescriptorCore desc, FlagType flags)
{
  XR_ASSERT(Asset::Manager, desc.type == T::kTypeId || desc.type == 0);
  if (!desc.HasType())
  {
    desc.type = T::kTypeId;
  }

  return FindOrCreateInternal<T>(desc, flags);
}

////==============================================================================
template<class T>
inline
Counted<T> Asset::Manager::FindOrCreate(Descriptor<T> const& desc, FlagType flags)
{
  return FindOrCreateInternal(desc, flags);
}

//==============================================================================
template<class T>
inline
Counted<T> Asset::Manager::CreateInternal(DescriptorCore const& desc, FlagType flags)
{
  Ptr asset(T::Create(desc.hash, flags));
  if (!CheckAllMaskBits(flags, UnmanagedFlag))
  {
    Manage(asset);
  }
  return asset.StaticCast<T>();
}

//==============================================================================
template<class T>
inline
Counted<T> Asset::Manager::FindOrCreateInternal(DescriptorCore const& desc, FlagType flags)
{
  Counted<T> asset;
  if (!CheckAllMaskBits(flags, UnmanagedFlag))
  {
    if (auto tmp = Find(desc))
    {
      asset.Reset(tmp->Cast<T>());
    }
  }

  if (!asset)
  {
    asset = CreateInternal<T>(desc, flags);
  }
  return asset;
}

} // XR

//==============================================================================
///@brief Class declaration decorator for concrete Asset types.
#define XR_ASSET_DECL(className)\
  using Ptr = xr::Counted<className>;\
\
  static TypeId const kTypeId;\
  static VersionType const kVersion;\
  static className* Create(HashType hash, FlagType flags) {\
    return new className(DescriptorCore(kTypeId, hash), flags);\
  }\
\
  className(Asset::DescriptorCore const& desc, FlagType flags)\
  : Asset(desc, flags)\
  {}\
\
  ~className()\
  {\
    Unload();\
  }

//==============================================================================
///@brief Definition decorator for your concrete Asset type.
///@param className name of your Asset type.
///@param id four character ID actually given as string literal (including fifth, null
/// terminator character; must be unique amongst all assets.
///@param version Serialization version of the asset; should be incremented
/// every time the _serialized_ format changes.
///@param extensions accepted file types that the Asset will be built from.
/// Must be unique.
#define XR_ASSET_DEF(className, id, version, extensions)\
  static_assert(std::is_same<std::decay<decltype(id[0])>::type, char>::value, "Type ID for " #className " must be chars.");\
  static_assert(XR_ARRAY_SIZE(id) == 5 && id[4] == '\0', "Type ID for " #className " must be 4 characters.");\
  static_assert((version) <= std::numeric_limits<xr::Asset::VersionType>::max(), "Version must be xr::Asset::VersionType.");\
\
  xr::Asset::TypeId const className::kTypeId = XR_FOURCC(id[0], id[1], id[2], id[3]);\
  xr::Asset::VersionType const className::kVersion = (version);\
\
  namespace {\
  xr::detail::AssetReflector xr ## className ## Reflector(className::kTypeId,\
    className::kVersion,\
    [](xr::Asset::HashType h, xr::Asset::FlagType f) -> xr::Asset* {\
      return className::Create(h, f);\
    }, extensions);\
  }

//==============================================================================
///@brief Facilitates the declaration of a concrete Asset::Builder which compiles out
/// when asset building is disabled. Place in .cpp
#ifdef ENABLE_ASSET_BUILDING
#define XR_ASSET_BUILDER_DECL(assetType)\
  class assetType##Builder: public xr::Asset::Builder\
  {\
  public:\
    assetType##Builder(): xr::Asset::Builder(assetType::kTypeId) {}\
    bool Build(char const* rawNameExt, Buffer buffer,\
      std::vector<FilePath>& dependencies, std::ostream& data) const override;\
  } s_builder##assetType;
#else
#define XR_ASSET_BUILDER_DECL(assetType)
#endif

//==============================================================================
///@brief Signature for the Build() function of @a assetType 's builder that
/// was declared with XR_ASSET_BUILDER_DECL.
#define XR_ASSET_BUILDER_BUILD_SIG(assetType)\
  bool assetType##Builder::Build(char const* rawNameExt, Buffer buffer,\
    std::vector<FilePath>& dependencies, std::ostream& data) const

#endif //XR_ASSET_HPP

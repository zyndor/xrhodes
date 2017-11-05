//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_ASSET_HPP
#define XR_ASSET_HPP

#include "XR/File.hpp"
#include "XR/Counted.hpp"
#include "XR/Counter.hpp"
#include "XR/Hash.hpp"
#include "XR/Linked.hpp"
#include "XR/fundamentals.hpp"
#include "XR/memory.hpp"
#include "XR/utils.hpp"
#include "XR/debug.hpp"
#include <memory>
#include <vector>

namespace XR
{

class FileWriter;

//==============================================================================
///@brief The Asset API provides facilities to create and asynchronously
/// load assets, while managing dependencies and ownership.
///@note Use XR_ASSET_DECL and XR_ASSET_DEF to facilitate implementation of
/// concrete Asset types.
class Asset: public Countable
{
public:
  // types
  using Ptr = Counted<Asset>;

  using FlagType = uint32_t;
  enum Flags: FlagType
  {
    // public
    ForceReloadFlag = XR_MASK_ID(FlagType, 0),  // Load() will not load an already loaded asset, unless this flag is specified.
    LoadSyncFlag = XR_MASK_ID(FlagType, 1),  // forces the asset to load synchronously.
    UnmanagedFlag = XR_MASK_ID(FlagType, 2), // the asset manager will not keep a reference to the asset.
    KeepSourceDataFlag = XR_MASK_ID(FlagType, 3), // source data is discarded by default; handling this flag is a responsibility of the given asset type

    // private
    LoadingFlag = XR_MASK_ID(FlagType, XR_BITSIZEOF(Flags) - 4),
    LoadedFlag = XR_MASK_ID(FlagType, XR_BITSIZEOF(Flags) - 3),
    ReadyFlag = XR_MASK_ID(FlagType, XR_BITSIZEOF(Flags) - 2),
    ErrorFlag = XR_MASK_ID(FlagType, XR_BITSIZEOF(Flags) - 1),  // The processing of the asset stops when this is set, and the rest of the private flags indicate where things have gone bad.

    PrivateMask = ErrorFlag | ReadyFlag | LoadedFlag | LoadingFlag
  };

  ///@brief Carries information about the type and identifier of an Asset.
  struct DescriptorCore
  {
    explicit DescriptorCore(uint32_t type_ = 0, uint64_t hash_ = 0)
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
      sprintf(hashBuffer, "%.16llx", hash);
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

    uint32_t type;
    uint64_t hash;
  };

  ///@brief Descriptor of a given type of Asset.
  template <class T>
  struct Descriptor final: DescriptorCore
  {
    using Type = T;

    explicit Descriptor(uint64_t hash_ = 0)
    : DescriptorCore(Type::kTypeId, hash_)
    {}
  };

  ///@brief Builders are responsible for converting raw assets into engine
  /// format. The Manager reads files and hands the data from those with
  /// the Builder's supported extension to them, along with a path to write
  /// the built asset to.
  class Builder: Linked<Builder>
  {
  public:
    // types
    using Base = Linked<Builder>;

    // structors
    Builder()
    : Base(*this)
    {}

    virtual ~Builder()
    {}

    ///@return ; separated list of extensions (not including '.'), that should
    /// be built by this Builder.
    virtual char const* GetExtensions() const = 0;

    ///@brief Parses @a buffer of size @a size bytes, and produces two buffers
    /// of @a dependencies and asset @a data for writing into the built asset.
    virtual bool Build(char const* rawNameExt, uint8_t const* buffer, size_t size,
      std::vector<FilePath>& dependencies, std::vector<uint8_t>& data) const = 0;

    ///@brief Determines what happens when trying to register a Builder for
    /// an extension that is already handled.
    /// - Overridable() builders will always be overridden by new registrations
    ///   (for conflicting extensions).
    /// - trying to register a non-Overridable() Builder over another one
    ///   causes an error.
    /// - requests to register an Overridable() Builder over a non-Overridable()
    ///   one are ignored.
    /// Builders in library code - such as all of the ones in XR3 - should be
    /// Overridable(); make the concrete ones that your application uses,
    /// non-Overridable().
    virtual bool Overridable() const { return true; }
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
    ///@note Raw assets' hash is calculated from their location, in the following
    /// way:
    /// - ram / rom and asset paths are stripped (but not the rest of the path).
    /// - file extension is stripped (the type is a separate part of the
    ///   descriptor).
    static uint64_t HashPath(FilePath path);

    ///@brief Attempts to load a built asset from the asset path that the
    /// given @a path hashes to, either asynchronously (Manager::Update() will
    /// need to be called regularly), or synchronously (if the LoadSyncFlag was
    /// set). If asset building is enabled then the actual @a path will be
    /// checked for a raw asset more recent than the built asset, and the asset
    /// will be rebuilt. This must succeed for the successful loading of the
    /// asset.
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
    static Ptr Load(Descriptor<T> const& desc, FlagType flags = 0)
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

    ///@brief Attempts to retrieve an asset of the given descriptor, from the
    /// map of managed assets.
    static Ptr Find(DescriptorCore const& desc);

    ///@brief Attempts to retrieve a asset of the given descriptor, from the
    /// map of managed assets.
    template <class T>
    static Counted<T> Find(Descriptor<T> const& desc);

    ///@brief Attempts to retrieve a asset of that the given @a path hashes to,
    /// from the map of managed assets.
    template <class T>
    static Counted<T> Find(FilePath const& path);

    ///@brief Attempts to retrieve a managed asset (unless opted out) or
    /// create one (and manage it - unless opted out).
    template <class T>
    static Counted<T> FindOrCreate(DescriptorCore desc, FlagType flags = 0);

    template <class T>
    static Counted<T> FindOrCreate(Descriptor<T> const& desc, FlagType flags = 0);

    ///@brief If @a asset wasn't managed, it'll add it to the map and clears the
    /// UnmanagedFlag on it.
    ///@return The success of the operation, i.e. if @a asset was the first with
    /// the given descriptor, to be managed.
    static bool Manage(Ptr asset);

    ///@brief If @a asset was managed, it'll remove it from the map and sets
    /// UnmanagedFlag on it.
    ///@return The success of the removal, i.e. if it was found in the map.
    static bool Remove(Asset& asset);

    ///@brief Calls Unload() on all assets who have a single reference, held by
    /// the Asset::Manager.
    ///@note This doesn't remove them from the manager.
    static void UnloadUnused();

    ///@brief Processes loaded assets and completes their loading, caling
    /// OnLoaded().
    static void Update();

    static void Suspend();
    static void Resume();

    static void Exit();

  private:
    // internal
    template <class T>
    static Counted<T> CreateInternal(DescriptorCore const& desc, FlagType flags);

    template <class T>
    static Counted<T> FindOrCreateInternal(DescriptorCore const& desc, FlagType flags);

    static bool IsLoadable(FlagType oldFlags, FlagType newFlags);

    static void LoadInternal(uint16_t version, FilePath const& path, Ptr const& asset,
      FlagType flags);
    static void LoadInternal(uint16_t version, Ptr const& asset, FlagType flags);
  };

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

  ///@brief Marks end of usage / dependency.
  bool Release()
  {
    bool expired;
    m_refs.Release(expired);
    return expired;
  }

  ///@brief Loads the asset with the given source data.
  bool Load(size_t size, uint8_t const* buffer);

  ///@brief Unloads the Asset (calling OnUnload()) only if it has been loaded.
  /// If it has, or if the ErrorFlag was set, the private flags are cleared.
  ///@return Whether unload has been performed.
  ///@note This doesn't invalidate the Asset object; it may be reloaded based
  /// on its descriptor.
  bool Unload();

protected:
  // types
  using Lock = Counter<Spinlocked>;

  // structors
  explicit Asset(DescriptorCore const& desc, uint32_t flags = 0)
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
  ///@brief Called asset data is loaded -- by Load().
  virtual bool OnLoaded(size_t size, uint8_t const* buffer) = 0;

  ///@brief Called when unloading of Asset data is requested.
  ///@note For the concrete resource type, this should happen on the same thread
  /// as you do the loading on.
  virtual void OnUnload() = 0;
};


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
  Ptr asset(T::Create(desc, flags));
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

#define XR_ASSET_DECL(className)\
  using Ptr = XR::Counted<className>;\
\
  static uint32_t const kTypeId;\
  static uint16_t const kVersion;\
  static className* Create(Asset::DescriptorCore const& desc, FlagType flags) {\
    return new className(desc, flags);\
  }\
  static Asset::Builder const& GetBuilder();\
\
  className(Asset::DescriptorCore const& desc, FlagType flags)\
  : Asset(desc, flags)\
  {}\
\
  ~className()\
  {\
    Unload();\
  }

#define XR_ASSET_DEF(className, id, version)\
  static_assert(std::is_same<std::decay<decltype(id[0])>::type, char>::value, "Type ID for " #className " must be chars.");\
  static_assert(XR_ARRAY_SIZE(id) == 5 && id[4] == '\0', "Type ID for " #className " must be 4 characters.");\
  static_assert((version) <= std::numeric_limits<uint16_t>::max(), "Version must be uint16_t.");\
\
  uint32_t const className::kTypeId = XR_FOURCC(id[0], id[1], id[2], id[3]);\
  uint16_t const className::kVersion = (version);\


#endif //XR_ASSET_HPP

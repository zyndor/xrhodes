//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_ASSET_HPP
#define XR_ASSET_HPP

#include <XR/File.hpp>
#include <XR/Counted.hpp>
#include <XR/Counter.hpp>
#include <XR/Hash.hpp>
#include <XR/fundamentals.hpp>
#include <XR/memory.hpp>
#include <XR/utils.hpp>
#include <XR/debug.hpp>
#include <memory>
#include <vector>

namespace XR
{

//==============================================================================
///@brief The Asset API provides facilities to create and asynchronously
/// load assets, while managing dependencies and ownership.
///@note Use XR_ASSET_DECL to facilitate implementation of concrete Asset types.
class Asset: public Countable
{
public:
  // types
  using Ptr = Counted<Asset>;

  using FlagType = uint32_t;
  enum Flags: FlagType
  {
    // public
    ForceReloadFlag = XR_MASK_ID(0),  // Load() will not load an already loaded asset, unless this flag is specified.
    LoadSyncFlag = XR_MASK_ID(1),  // forces the asset to load synchronously.
    UnmanagedFlag = XR_MASK_ID(2), // the asset manager will not keep a reference to the asset.
    KeepSourceDataFlag = XR_MASK_ID(3), // source data is discarded by default; handling this flag is a responsibility of the given asset type

    // private
    LoadingFlag = XR_MASK_ID(XR_BITSIZEOF(Flags) - 4),
    LoadedFlag = XR_MASK_ID(XR_BITSIZEOF(Flags) - 3),
    ReadyFlag = XR_MASK_ID(XR_BITSIZEOF(Flags) - 2),
    ErrorFlag = XR_MASK_ID(XR_BITSIZEOF(Flags) - 1),  // The processing of the asset stops when this is set, and the rest of the private flags indicate where things have gone bad.

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
  class Builder
  {
  public:
    virtual ~Builder() {}

    ///@return ; separated list of extensions (not including '.'), that should
    /// be built by this Builder.
    virtual char const* GetExtensions() const = 0;

    ///@brief Performs the building and writing of the asset into the file 
    /// handle provided.
    virtual bool Build(uint8_t const* buffer, size_t size, FilePath targetPath) const = 0;
  };

  ///@brief Offers synchronous and asynchronous loading, and maintains a map,
  /// of assets.
  struct Manager
  {
    ///@brief Initialises the Asset::Manager with the given asset path.
    /// This is where built assets are expected to be (and will be saved by
    /// the asset build pipeline). It will be stripped of ram/rom roots.
    ///@note This means that {ram}/{assets}/ as well as {rom}/{assets}/ should
    /// not contain raw assets or any other data - the former at the risk of
    /// being ignored, and both at the risk of being overwritten.
    ///@note If an empty path is supplied, "assets/" is going to be used.
    static void Init(FilePath const& path = "", Allocator* alloc = nullptr);

    static void RegisterBuilder(Builder const& builder);

    static const FilePath& GetAssetPath();

    ///@brief Calculates the hash of the given path.
    ///@note If the file is in the asset path (given to Init()), then it
    /// must be a valid built asset.
    ///@note Raw assets' hash is calculated from their stripped location
    /// (File::StripRoots()), and no extension (the type that it should map
    /// to is a separate part of the descriptor).
    static uint64_t HashPath(FilePath path);

    ///@brief Adds a asset at the given location, to the loading queue to
    /// be loaded (or, if LoadSyncFlag was set, loads it synchronously).
    template <class T>
    static Counted<T> Load(FilePath path, FlagType flags = 0)
    {
      flags &= ~PrivateMask;
      Descriptor<T> desc(HashPath(path));
      Counted<T> asset = FindOrCreateInternal<T>(desc, flags);

      auto foundFlags = asset->GetFlags();
      if (IsLoadable(foundFlags, flags))
      {
        LoadInternal(path, asset.StaticCast<Asset>(), flags);
      }
      return asset;
    }

    ///@brief Loads a built asset.
    template <class T>
    static Ptr Load(Descriptor<T> const& desc, FlagType flags = 0)
    {
      flags &= ~PrivateMask;
      Counted<T> asset = FindOrCreateInternal<T>(desc, flags);

      auto foundFlags = asset->GetFlags();
      if (IsLoadable(foundFlags, flags))
      {
        LoadInternal(asset.StaticCast<Asset>(), flags);
      }
      return asset;
    }

    ///@brief Attempts to retrieve a asset of the given descriptor, from the
    /// map of managed assets.
    static Ptr Find(DescriptorCore const& desc);

    template <class T>
    static Counted<T> Find(Descriptor<T> const& desc);

    template <class T>
    static Counted<T> Find(FilePath const& path);

    ///@brief Attempts to retrieve a managed asset (unless opted out) or
    /// create one (and manage it - unless opted out).
    template <class T>
    static Counted<T> FindOrCreate(DescriptorCore desc, FlagType flags = 0);

    template <class T>
    static Counted<T> FindOrCreate(Descriptor<T> const& desc, FlagType flags = 0);

    ///@brief If @a asset wasn't managed, it'll add it to the map and updates the
    /// appropriate flag on it.
    ///@return The success of the operation, i.e. if @a asset was the first with
    /// the given descriptor, to be managed.
    static bool Manage(Ptr asset);

    static bool Unload(Asset& asset);

    ///@brief Processes loaded assets and completes their loading.
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

    static void LoadInternal(FilePath const& path, Ptr const& asset, FlagType flags);
    static void LoadInternal(Ptr const& asset, FlagType flags);
  };

  // structors
  virtual ~Asset()
  {}

  // virtual
  ///@brief Called by the Asset::Manager when the asset data is loaded.
  virtual bool OnLoaded(size_t size, uint8_t const* buffer) = 0;
  virtual void OnUnloaded() = 0;

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

  ///@brief Marks a usage or dependency and prevents this asset from being unloaded.
  void Acquire()
  {
    m_refs.Acquire();
  }

  ///@brief Marks end of usage / dependency and enables this asset to be unloaded.
  bool Release()
  {
    bool expired;
    m_refs.Release(expired);
    return expired;
  }

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
  if (!IsFullMask(flags, UnmanagedFlag))
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
  if (!IsFullMask(flags, UnmanagedFlag))
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

#define XR_ASSET_DECL(className, id)\
  using Ptr = XR::Counted<className>;\
\
  static_assert(std::is_same<std::decay<decltype(id[0])>::type, char>::value, "Type ID for " #className " must be chars.");\
  static_assert(XR_ARRAY_SIZE(id) == 5 && id[4] == '\0', "Type ID for " #className " must be 4 characters.");\
\
  static uint32_t const kTypeId = XR_FOURCC(id[0], id[1], id[2], id[3]);\
  static className* Create(Asset::DescriptorCore const& desc, FlagType flags) {\
    return new className(desc, flags);\
  }\
  static Asset::Builder const& GetBuilder();\
\
  className(Asset::DescriptorCore const& desc, FlagType flags)\
  : Asset(desc, flags)\
  {}


#endif //XR_ASSET_HPP

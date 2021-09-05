#ifndef XR_ASSETPACK_HPP
#define XR_ASSETPACK_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Asset.hpp"
#include "xr/memory/Queue.hpp"
#include "xr/utility/Hash.hpp"
#include <unordered_map>

namespace xr
{

//==============================================================================
///@brief AssetPack is an aggregate of Assets that are defined in it, with
/// their dependencies consolidated. Each asset may have an alias defined,
/// which can be used to retrieve it from the AssetPack.
///@par Additionally, the AssetPack keeps a reference to all of its assets,
/// thereby preventing them being unloaded as unused (until the AssetPack itself
/// is unloaded).
///@par Format:
/// {
///   "path/to/asset.x",
///   "my_alias": "path/to/asset.y"
/// }
///@note Multiply defined assets are consolidated and written only once.
///@note Multiple aliases for the same asset are ignored, and the only the latest
/// one is used.
///@note The AssetPack adds all Assets to the Asset::Manager. If the UnmanagedFlag
/// was specified, then only the AssetPack will not be managed.
class AssetPack: public Asset
{
public:
  XR_ASSET_DECL(AssetPack)

  // general
  ///@brief Attempts to get an Asset by the alias that it was given.
  ///@note Assets loaded by this AssetPack can be retrieved directly from
  /// Asset::Manager, using their path / hash.
  [[deprecated("Use GetAssetPtr.")]]
  Asset::Ptr Get(HashType alias) const
  {
    return GetAssetPtr(alias);
  }

  ///@brief Attempts to get an Asset by the alias that it was given.
  ///@note Assets loaded by this AssetPack can be retrieved directly from
  /// Asset::Manager, using their path / hash.
  [[deprecated("Use GetAssetPtr.")]]
  Asset::Ptr Get(const char* alias) const
  {
    return GetAssetPtr(alias);
  }

  ///@brief Attempts to get an Asset by the hash of the alias that it was given
  /// in the pack.
  ///@note Assets loaded by this AssetPack may be retrieved directly from
  /// Asset::Manager, using their path / hash (unless UnmanagedFlag was set).
  Asset::Ptr GetAssetPtr(HashType alias) const;

  ///@brief Attempts to get an Asset by the alias that it was given in the pack.
  ///@note Assets loaded by this AssetPack may be retrieved directly from
  /// Asset::Manager, using their path / hash (unless UnmanagedFlag was set).
  Asset::Ptr GetAssetPtr(const char* alias) const;

  ///@brief Attempts to get an Asset of type T, by the hash of the alias that it
  /// was given in the pack.
  ///@note Assets loaded by this AssetPack may be retrieved directly from
  /// Asset::Manager, using their path / hash (unless UnmanagedFlag was set).
  template <typename T>
  typename T::Ptr GetAsset(HashType alias) const
  {
    typename T::Ptr result;
    if (auto ptr = GetAssetPtr(alias))
    {
      result.Reset(ptr->Cast<T>());
    }
    return result;
  }

  ///@brief Attempts to get an Asset of type T, by the alias that it was given
  /// in the pack.
  ///@note Assets loaded by this AssetPack may be retrieved directly from
  /// Asset::Manager, using their path / hash (unless UnmanagedFlag was set).
  template <typename T>
  typename T::Ptr GetAsset(const char* alias) const
  {
    return GetAsset<T>(Hash::String(alias));
  }

private:
  // types
  struct IdentityHash
  {
    HashType operator()(HashType h) const
    {
      return h;
    }
  };

  // data
  std::unordered_map<HashType, Asset::Ptr, IdentityHash> m_aliased;
  Queue<Asset::Ptr> m_unnamed; // ownership only; they're never actually retrieved from the pack.

  // internal
  bool OnLoaded(Buffer buffer) override;
  void OnUnload() override;
};

} // xr

#endif

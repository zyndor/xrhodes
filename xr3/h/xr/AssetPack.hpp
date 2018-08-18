#ifndef XR_ASSETPACK_HPP
#define XR_ASSETPACK_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Asset.hpp"
#include "xr/Queue.hpp"
#include <map>

namespace xr
{

//==============================================================================
///@brief AssetPack is an aggregate of Assets that are defined in it, with
/// their dependencies consolidated. Each asset may have an alias defined,
/// which can be used to retrieve it from the AssetPack.
/// Additionally, the AssetPack keeps a reference to all of its assets,
/// thereby preventing them being unloaded as unused (until the AssetPack itself
/// is unloaded).
/// The AssetPack adds all Assets to the Asset::Manager. If the UnmanagedFlag
/// was specified, then only the AssetPack and any dependencies will not be
/// managed.
/// Format:
/// {
///   "path/to/asset.x",
///   "my_alias": "path/to/asset.y"
/// }
class AssetPack: public Asset
{
public:
  XR_ASSET_DECL(AssetPack)

  // general
  ///@brief Attempts to get an Asset by the alias that it was given.
  ///@note Assets loaded by this AssetPack can be retrieved directly from
  /// Asset::Manager, using their path / hash.
  Asset::Ptr Get(HashType alias) const;

  ///@brief Attempts to get an Asset by the alias that it was given.
  ///@note Assets loaded by this AssetPack can be retrieved directly from
  /// Asset::Manager, using their path / hash.
  Asset::Ptr Get(const char* alias) const;

private:
  // data
  std::map<HashType, Asset::Ptr> m_aliased;
  Queue<Asset::Ptr> m_unnamed; // ownership only; they're never actually retrieved from the pack.

  // internal
  bool OnLoaded(Buffer buffer) override;
  void OnUnload() override;
};

} // xr

#endif

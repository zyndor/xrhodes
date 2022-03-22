//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "FileLifeCycleManager.hpp"
#include "xr/AssetPack.hpp"
#include "xr/Device.hpp"
#include "xr/Material.hpp"
#include "xm.hpp"

using namespace xr;

namespace
{

#ifdef ENABLE_ASSET_BUILDING
class AssetPack
{
public:
  AssetPack()
  {
    xr::Device::Init();

    Asset::Manager::Init(".assets");
  }

  ~AssetPack()
  {
    Asset::Manager::Shutdown();

    Device::Shutdown();
  }

private:
  FileLifeCycleManager  flcm;
};

XM_TEST_F(AssetPack, Contents)
{
  // check assets from the pack - not present.
  Asset::Ptr randomAsset(Asset::Manager::Find<Texture>("assets/xrhodes.tex").Get());
  XM_ASSERT_FALSE(randomAsset);

  // load pack
  auto pack = Asset::Manager::Load<xr::AssetPack>("assets/assets.pak", Asset::DryRunFlag | Asset::LoadSyncFlag | Asset::ForceBuildFlag);

  // check pack on manager - present
  XM_ASSERT_EQ(pack->GetRefCount(), 2);  // us, manager
  randomAsset.Reset(Asset::Manager::Find<xr::AssetPack>("assets/assets.pak").Get());
  XM_ASSERT_TRUE(bool(randomAsset));

  // check dependency of asset from the pack - present.
  randomAsset.Reset(Asset::Manager::Find<Shader>("assets/xrhodes.shd").Get());
  XM_ASSERT_TRUE(bool(randomAsset));
  XM_ASSERT_EQ(randomAsset->GetDescriptor().type, Shader::kTypeId);
  XM_ASSERT_EQ(randomAsset->GetRefCount(), 3); // us, material, manager

  // check asset from the pack -- present.
  randomAsset.Reset(Asset::Manager::Find<Material>("assets/xrhodes.mtl").Get());
  XM_ASSERT_TRUE(bool(randomAsset));
  XM_ASSERT_EQ(randomAsset->GetDescriptor().type, Material::kTypeId);
  XM_ASSERT_EQ(randomAsset->GetRefCount(), 3); // us, pack, manager

  // getters work
  XM_ASSERT_EQ(randomAsset.Get(), pack->GetAssetPtr("logo").Get());
  XM_ASSERT_EQ(randomAsset.Get(), pack->GetAsset<Material>("logo").Get());
}

XM_TEST_F(AssetPack, ContentsUnmanaged)
{
  // check assets from the pack - not present.
  Asset::Ptr randomAsset(Asset::Manager::Find<Texture>("assets/xrhodes.png").Get());
  XM_ASSERT_FALSE(bool(randomAsset));

  // load pack
  auto pack = Asset::Manager::Load<xr::AssetPack>("assets/assets.pak", Asset::UnmanagedFlag | Asset::DryRunFlag | Asset::LoadSyncFlag | Asset::ForceBuildFlag);

  // check pack on manager - not present
  XM_ASSERT_EQ(pack->GetRefCount(), 1);  // us only
  XM_ASSERT_FALSE(bool(Asset::Manager::Find<xr::AssetPack>("assets/assets.pak")));

  // check dependency of asset from pack -- present.
  randomAsset.Reset(Asset::Manager::Find<Shader>("assets/xrhodes.shd").Get());
  XM_ASSERT_TRUE(bool(randomAsset));
  XM_ASSERT_EQ(randomAsset->GetRefCount(), 3); // us, material, manager

  // check asset from pack -- present.
  randomAsset.Reset(Asset::Manager::Find<Material>("assets/xrhodes.mtl").Get());
  XM_ASSERT_TRUE(bool(randomAsset));
  XM_ASSERT_EQ(randomAsset->GetDescriptor().type, Material::kTypeId);
  XM_ASSERT_EQ(randomAsset->GetRefCount(), 3); // us, pack, manager

  // check dependency via asset -- present.
  randomAsset.Reset(randomAsset->Cast<Material>()->GetTexture(0).Get());
  XM_ASSERT_TRUE(bool(randomAsset));
  XM_ASSERT_EQ(randomAsset->GetRefCount(), 4); // us, material, pack, manager
}

XM_TEST_F(AssetPack, Aliases)
{
  // load pack
  auto pack = Asset::Manager::Load<xr::AssetPack>("assets/assets.pak", Asset::DryRunFlag | Asset::LoadSyncFlag | Asset::ForceBuildFlag);

  // check assets from the pack - now present.
  auto randomAsset = pack->GetAssetPtr("logo");
  XM_ASSERT_TRUE(bool(randomAsset));
  XM_ASSERT_EQ(randomAsset->GetDescriptor().type, Material::kTypeId);
  const auto hash = randomAsset->GetDescriptor().hash;

  randomAsset = pack->GetAssetPtr(Hash::String("logo"));
  XM_ASSERT_TRUE(bool(randomAsset));
  XM_ASSERT_EQ(randomAsset->GetDescriptor().type, Material::kTypeId);
  XM_ASSERT_EQ(randomAsset->GetDescriptor().hash, hash); // same asset

  // asset by overridden alias - not found
  randomAsset = pack->GetAssetPtr(Hash::String("mytex"));
  XM_ASSERT_FALSE(bool(randomAsset));

  // asset by new alias - found
  randomAsset = pack->GetAssetPtr(Hash::String("texture"));
  XM_ASSERT_TRUE(bool(randomAsset));
}

#endif
}


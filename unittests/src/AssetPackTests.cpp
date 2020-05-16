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
#include "gtest/gtest.h"

using namespace xr;

namespace
{

#ifdef ENABLE_ASSET_BUILDING
class AssetPack : public ::testing::Test
{
public:
  static void SetUpTestCase()
  {}

  static void TearDownTestCase()
  {}

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

TEST_F(AssetPack, Contents)
{
  // check assets from the pack - not present.
  Asset::Ptr randomAsset(Asset::Manager::Find<Texture>("assets/xrhodes.tex").Get());
  ASSERT_FALSE(randomAsset);

  // load pack
  auto pack = Asset::Manager::Load<xr::AssetPack>("assets/assets.pak", Asset::DryRunFlag | Asset::LoadSyncFlag | Asset::ForceBuildFlag);

  // check pack on manager - present
  ASSERT_EQ(pack->GetRefCount(), 2);  // us, manager
  ASSERT_TRUE(Asset::Manager::Find<xr::AssetPack>("assets/assets.pak"));

  // check dependency of asset from the pack - present.
  randomAsset.Reset(Asset::Manager::Find<Shader>("assets/xrhodes.shd").Get());
  ASSERT_TRUE(randomAsset);
  ASSERT_EQ(randomAsset->GetDescriptor().type, Shader::kTypeId);
  ASSERT_EQ(randomAsset->GetRefCount(), 3); // us, material, manager

  // check asset from the pack -- present.
  randomAsset.Reset(Asset::Manager::Find<Material>("assets/xrhodes.mtl").Get());
  ASSERT_TRUE(randomAsset);
  ASSERT_EQ(randomAsset->GetDescriptor().type, Material::kTypeId);
  ASSERT_EQ(randomAsset->GetRefCount(), 3); // us, pack, manager

  // getters work
  ASSERT_EQ(randomAsset.Get(), pack->GetAssetPtr("logo").Get());
  ASSERT_EQ(randomAsset.Get(), pack->GetAsset<Material>("logo").Get());
}

TEST_F(AssetPack, ContentsUnmanaged)
{
  // check assets from the pack - not present.
  Asset::Ptr randomAsset(Asset::Manager::Find<Texture>("assets/xrhodes.png").Get());
  ASSERT_FALSE(randomAsset);

  // load pack
  auto pack = Asset::Manager::Load<xr::AssetPack>("assets/assets.pak", Asset::UnmanagedFlag | Asset::DryRunFlag | Asset::LoadSyncFlag | Asset::ForceBuildFlag);

  // check pack on manager - not present
  ASSERT_EQ(pack->GetRefCount(), 1);  // us only
  ASSERT_FALSE(Asset::Manager::Find<xr::AssetPack>("assets/assets.pak"));

  // check dependency of asset from pack -- present.
  randomAsset.Reset(Asset::Manager::Find<Shader>("assets/xrhodes.shd").Get());
  ASSERT_TRUE(randomAsset);
  ASSERT_EQ(randomAsset->GetRefCount(), 3); // us, material, manager

  // check asset from pack -- present.
  randomAsset.Reset(Asset::Manager::Find<Material>("assets/xrhodes.mtl").Get());
  ASSERT_TRUE(randomAsset);
  ASSERT_EQ(randomAsset->GetDescriptor().type, Material::kTypeId);
  ASSERT_EQ(randomAsset->GetRefCount(), 3); // us, pack, manager

  // check dependency via asset -- present.
  randomAsset.Reset(randomAsset->Cast<Material>()->GetTexture(0).Get());
  ASSERT_TRUE(randomAsset);
  ASSERT_EQ(randomAsset->GetRefCount(), 4); // us, material, pack, manager
}

TEST_F(AssetPack, Aliases)
{
  // load pack
  auto pack = Asset::Manager::Load<xr::AssetPack>("assets/assets.pak", Asset::DryRunFlag | Asset::LoadSyncFlag | Asset::ForceBuildFlag);

  // check assets from the pack - now present.
  auto randomAsset = pack->GetAssetPtr("logo");
  ASSERT_TRUE(randomAsset);
  ASSERT_EQ(randomAsset->GetDescriptor().type, Material::kTypeId);
  const auto hash = randomAsset->GetDescriptor().hash;

  randomAsset = pack->GetAssetPtr(Hash::String("logo"));
  ASSERT_TRUE(randomAsset);
  ASSERT_EQ(randomAsset->GetDescriptor().type, Material::kTypeId);
  ASSERT_EQ(randomAsset->GetDescriptor().hash, hash); // same asset

  // asset by overridden alias - not found
  randomAsset = pack->GetAssetPtr(Hash::String("mytex"));
  ASSERT_FALSE(randomAsset);

  // asset by new alias - found
  randomAsset = pack->GetAssetPtr(Hash::String("texture"));
  ASSERT_TRUE(randomAsset);
}

#endif
}


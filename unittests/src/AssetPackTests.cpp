#include "FileLifeCycleManager.hpp"
#include "xr/AssetPack.hpp"
#include "xr/Device.hpp"
#include "xr/Material.hpp"
#include "gtest/gtest.h"

using namespace xr;

#ifdef ENABLE_ASSET_BUILDING
class AssetPackTests : public ::testing::Test
{
public:
  static void SetUpTestCase()
  {}

  static void TearDownTestCase()
  {}

  AssetPackTests()
  {
    xr::Device::Init();

    Asset::Manager::Init(".assets");
  }

  ~AssetPackTests()
  {
    Asset::Manager::Shutdown();

    Device::Shutdown();
  }

private:
  FileLifeCycleManager  flcm;
};

TEST_F(AssetPackTests, Contents)
{
  // check assets from the pack - not present.
  Asset::Ptr randomAsset(Asset::Manager::Find<Texture>("assets/xrhodes.png").Get());
  ASSERT_FALSE(randomAsset);

  // load pack
  auto pack = Asset::Manager::Load<AssetPack>("assets/assets.pak", Asset::DryRunFlag | Asset::LoadSyncFlag);

  // check dependency of asset from the pack - now present.
  randomAsset.Reset(Asset::Manager::Find<Texture>("assets/xrhodes.png").Get());
  ASSERT_TRUE(randomAsset);
  ASSERT_EQ(randomAsset->GetDescriptor().type, Texture::kTypeId);
  ASSERT_EQ(randomAsset->GetRefCount(), 3); // us, material, manager

  // check asset from the pack -- present.
  randomAsset.Reset(Asset::Manager::Find<Material>("assets/xrhodes.mtl").Get());
  ASSERT_TRUE(randomAsset);
  ASSERT_EQ(randomAsset->GetDescriptor().type, Material::kTypeId);
  ASSERT_EQ(randomAsset->GetRefCount(), 3); // us, manager, pack
}

TEST_F(AssetPackTests, ContentsUnmanaged)
{
  // check assets from the pack - not present.
  Asset::Ptr randomAsset(Asset::Manager::Find<Texture>("assets/xrhodes.png").Get());
  ASSERT_FALSE(randomAsset);

  // load pack
  auto pack = Asset::Manager::Load<AssetPack>("assets/assets.pak", Asset::UnmanagedFlag | Asset::DryRunFlag | Asset::LoadSyncFlag);

  // check dependency on manager -- not present.
  randomAsset.Reset(Asset::Manager::Find<Texture>("assets/xrhodes.png").Get());
  ASSERT_FALSE(randomAsset);

  // check asset from pack -- present.
  randomAsset.Reset(Asset::Manager::Find<Material>("assets/xrhodes.mtl").Get());
  ASSERT_TRUE(randomAsset);
  ASSERT_EQ(randomAsset->GetDescriptor().type, Material::kTypeId);
  ASSERT_EQ(randomAsset->GetRefCount(), 3); // us, pack, manager

  // check dependency via asset -- present.
  randomAsset.Reset(randomAsset->Cast<Material>()->GetTexture(0).Get());
  ASSERT_TRUE(randomAsset);
  ASSERT_EQ(randomAsset->GetRefCount(), 2); // us, material
}

TEST_F(AssetPackTests, Aliases)
{
  // load pack
  auto pack = Asset::Manager::Load<AssetPack>("assets/assets.pak", Asset::DryRunFlag | Asset::LoadSyncFlag);

  // check assets from the pack - now present.
  auto randomAsset = pack->Get("logo");
  ASSERT_TRUE(randomAsset);
  ASSERT_EQ(randomAsset->GetDescriptor().type, Material::kTypeId);
  const auto hash = randomAsset->GetDescriptor().hash;

  randomAsset = pack->Get(Hash::String("logo"));
  ASSERT_TRUE(randomAsset);
  ASSERT_EQ(randomAsset->GetDescriptor().type, Material::kTypeId);
  ASSERT_EQ(randomAsset->GetDescriptor().hash, hash); // same asset
}

#endif

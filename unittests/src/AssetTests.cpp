#include "FileLifeCycleManager.hpp"

#include "gtest/gtest.h"
#include "XR/Asset.hpp"
#include "XR/Hash.hpp"
#include "XR/FileWriter.hpp"

#include <random>

namespace XR
{
  template <typename T>
  static void AssertEq(T const& a, T const& b)
  {
    ASSERT_EQ(a, b);
  }

  static void AssertStrEq(char const* a, char const* b)
  {
    ASSERT_STREQ(a, b);
  }

  class AssetTests : public ::testing::Test
  {
  public:
    static void SetUpTestCase()
    {}

    static void TearDownTestCase()
    {}

    AssetTests()
    {
      Asset::Manager::Init(".assets");
    }

    ~AssetTests()
    {
      Asset::Manager::Exit();
    }

  private:
    FileLifeCycleManager  flcm;
  };

  // Test Asset and Builder
  struct TestAsset : Asset
  {
    XR_ASSET_DECL(TestAsset)

    virtual bool OnLoaded(size_t size, uint8_t const* buffer) override
    {
      AssertEq(size, sizeof(histogram));
      for (int i = 0; i < XR_ARRAY_SIZE(histogram); ++i)
      {
        int x = reinterpret_cast<int const*>(buffer)[i];
        histogram[i] = x;
      }

      return true;
    }

    virtual void OnUnload() override
    {
    }

    int histogram[256];
  };

  XR_ASSET_DEF(TestAsset, "tes7", 1, "test")

  XR_ASSET_BUILDER_DECL(TestAsset)

#ifdef ENABLE_ASSET_BUILDING
  XR_ASSET_BUILDER_BUILD_SIG(TestAsset)
  {
    AssertStrEq(rawNameExt, "testasset.test");

    data.resize(256 * sizeof(int), 0);
    int* histogram = reinterpret_cast<int*>(data.data());

    auto end = buffer + size;
    while (buffer < end)
    {
      ++histogram[*buffer];
      ++buffer;
    }

    return true;
  }
#endif

  void EnsureTestAssetExists(FilePath const& path)
  {
    if (!File::CheckExists(path))
    {
      std::random_device rd;
      std::mt19937 gen(rd());

      std::uniform_int_distribution<uint32_t> distro;

      FileWriter fw;
      FilePath rawPath(File::kRawProto + File::GetRomPath() / path);
      ASSERT_TRUE(fw.Open(rawPath, XR::FileWriter::Mode::Truncate, false));
      for (int i = 0; i < 2560000; ++i)
      {
        auto val = distro(gen);
        fw.Write(&val, sizeof(val), 1);
      }
    }
  }

  TEST_F(AssetTests, Basics)
  {
    FilePath path("assets/testasset.test");
    EnsureTestAssetExists(path);

    auto testAss = Asset::Manager::Load<TestAsset>(path);

    ASSERT_TRUE(CheckAllMaskBits(testAss->GetFlags(), Asset::LoadingFlag)); // load in progress
    ASSERT_EQ(Asset::Manager::Find<TestAsset>(path), testAss);  // manager has reference and is same

    while (!(testAss->GetFlags() & (Asset::ReadyFlag | Asset::ErrorFlag)))
    {
      Asset::Manager::Update();
    }
    ASSERT_FALSE(CheckAllMaskBits(testAss->GetFlags(), Asset::ErrorFlag));  // loaded successfully

    ASSERT_EQ(testAss->GetRefCount(), 2); // two references: one in manager, one local
    Asset::Manager::UnloadUnused();

    ASSERT_TRUE(CheckAllMaskBits(testAss->GetFlags(), Asset::ReadyFlag)); // still loaded
    auto desc = testAss->GetDescriptor();

    testAss.Reset(nullptr);

    Asset::Manager::UnloadUnused();
    auto cp = Asset::Manager::Find(desc);
    ASSERT_EQ(cp->GetRefCount(), 2);  // not removed
    ASSERT_FALSE(CheckAllMaskBits(cp->GetFlags(), Asset::ReadyFlag)); // unloaded
  }

  TEST_F(AssetTests, LoadReflected)
  {
    FilePath path("assets/testasset.test");
    EnsureTestAssetExists(path);

    auto testAss = Asset::Manager::LoadReflected(path);
    
    ASSERT_TRUE(testAss->Cast<TestAsset>()); // determined correct type
    ASSERT_TRUE(CheckAllMaskBits(testAss->GetFlags(), Asset::LoadingFlag)); // load in progress
    ASSERT_EQ(Asset::Manager::Find<TestAsset>(path), testAss);  // manager has reference and is same

    while (!(testAss->GetFlags() & (Asset::ReadyFlag | Asset::ErrorFlag)))
    {
      Asset::Manager::Update();
    }
    ASSERT_FALSE(CheckAllMaskBits(testAss->GetFlags(), Asset::ErrorFlag));  // loaded successfully
  }
}
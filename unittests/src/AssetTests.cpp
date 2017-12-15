#include "FileLifeCycleManager.hpp"

#include "gtest/gtest.h"
#include "XR/Asset.hpp"
#include "XR/Hash.hpp"
#include "XR/BufferReader.hpp"
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

    virtual bool OnLoaded(Buffer buffer) override
    {
      AssertEq(buffer.size, sizeof(histogram));
      for (int i = 0; i < XR_ARRAY_SIZE(histogram); ++i)
      {
        int x = reinterpret_cast<int const*>(buffer.data)[i];
        histogram[i] = x;
      }

      return true;
    }

    virtual void OnUnload() override
    {
    }

    int histogram[256];
  };

  XR_ASSET_DEF(TestAsset, "xUta", 1, "testBasic")

  XR_ASSET_BUILDER_DECL(TestAsset)

#ifdef ENABLE_ASSET_BUILDING
  XR_ASSET_BUILDER_BUILD_SIG(TestAsset)
  {
    AssertStrEq(rawNameExt, "testasset.testBasic");

    int histogram[256] = {};

    BufferReader reader(buffer);
    while (reader.GetRemainingSize() > 0)
    {
      ++histogram[*reader.ReadBytes(1)];
    }

    data.write(reinterpret_cast<char*>(histogram), sizeof(histogram));
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
    FilePath path("assets/testasset.testBasic");
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
    FilePath path("assets/testasset.testBasic");
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

  struct DependantTestAsset : public XR::Asset
  {
    XR_ASSET_DECL(DependantTestAsset)

    static std::vector<Asset::DescriptorCore> s_order;

    // Inherited via Asset
    virtual bool OnLoaded(Buffer buffer) override
    {
      s_order.push_back(GetDescriptor());
      return true;
    }

    virtual void OnUnload() override
    {
    }
  };

  XR_ASSET_DEF(DependantTestAsset, "xUda", 2, "testDeps")

  XR_ASSET_BUILDER_DECL(DependantTestAsset)

  std::vector<Asset::DescriptorCore> DependantTestAsset::s_order;

#ifdef ENABLE_ASSET_BUILDING
  XR_ASSET_BUILDER_BUILD_SIG(DependantTestAsset)
  {
    auto start = buffer.data;
    bool eol = !buffer.data || (*buffer.data == '\r' || *buffer.data == '\n') || buffer.size == 0;
    FilePath path;
    while (buffer.size > 0)
    {
      ++buffer.data;
      --buffer.size;

      bool isEol = (*buffer.data == '\r' || *buffer.data == '\n') || buffer.size == 0;
      if (isEol)
      {
        if(!eol)
        {
          path.assign((char const*)start, buffer.data - start);
          dependencies.push_back(path);
          start = buffer.data;
        }
        ++start;
      }
      eol = isEol;
    }

    return true;
  }
#endif

  TEST_F(AssetTests, Dependencies)
  {
    // Load 4 assets of the following dependency structure:
    // test -> test1, test2; test1 -> test3; test2 -> test3; test3
    DependantTestAsset::s_order.clear();

    FilePath path("assets/test.testDeps");

    // none of the dependencies exist
    ASSERT_EQ(Asset::Manager::Find<DependantTestAsset>("assets/test1.testDeps"), Asset::Ptr());
    ASSERT_EQ(Asset::Manager::Find<DependantTestAsset>("assets/test2.testDeps"), Asset::Ptr());
    ASSERT_EQ(Asset::Manager::Find<DependantTestAsset>("assets/test3.testDeps"), Asset::Ptr());

    auto testAss = Asset::Manager::Load<DependantTestAsset>(path, 0);

    ASSERT_TRUE(CheckAllMaskBits(testAss->GetFlags(), Asset::LoadingFlag)); // load in progress

    // dependencies are created - we don't care if they're loading, as this might
    // be done by the time we've looked them up and checked it (since only the
    // data loading is async and the dependencies are processed synchronously).
    auto dep1 = Asset::Manager::Find<DependantTestAsset>("assets/test1.testDeps");
    auto dep2 = Asset::Manager::Find<DependantTestAsset>("assets/test2.testDeps");
    auto dep3 = Asset::Manager::Find<DependantTestAsset>("assets/test3.testDeps");
    ASSERT_NE(dep1, Asset::Ptr());
    ASSERT_NE(dep2, Asset::Ptr());
    ASSERT_NE(dep3, Asset::Ptr());

    while (!(testAss->GetFlags() & (Asset::ReadyFlag | Asset::ErrorFlag)))
    {
      Asset::Manager::Update();
    }

    ASSERT_TRUE(CheckAllMaskBits(testAss->GetFlags(), Asset::ReadyFlag));
    ASSERT_TRUE(CheckAllMaskBits(dep1->GetFlags(), Asset::ReadyFlag));
    ASSERT_TRUE(CheckAllMaskBits(dep2->GetFlags(), Asset::ReadyFlag));
    ASSERT_TRUE(CheckAllMaskBits(dep3->GetFlags(), Asset::ReadyFlag));

    // Dependencies are loaded in the correct order - first the ones without any dependencies,
    // then the ones that depend upon them etc.
    // We can't inspect their loading directly, however we know that the order that their
    // OnLoaded() is called is the order they're popped off the loading queue.
    ASSERT_EQ(DependantTestAsset::s_order[0], dep3->GetDescriptor());
    ASSERT_EQ(DependantTestAsset::s_order[1], dep1->GetDescriptor());
    ASSERT_EQ(DependantTestAsset::s_order[2], dep2->GetDescriptor());
    ASSERT_EQ(DependantTestAsset::s_order[3], testAss->GetDescriptor());
  }
}
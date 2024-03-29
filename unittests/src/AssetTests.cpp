//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "FileLifeCycleManager.hpp"

#include "xm.hpp"
#include "xr/Asset.hpp"
#include "xr/memory/BufferReader.hpp"
#include "xr/FileWriter.hpp"

#include <random>

using namespace xr;

namespace
{

class Asset
{
public:
  Asset()
  {
    xr::Asset::Manager::Init(".assets");
  }

  ~Asset()
  {
    xr::Asset::Manager::Shutdown();
  }

private:
  FileLifeCycleManager  flcm;
};

// Test Asset and Builder
struct TestAsset : xr::Asset
{
  XR_ASSET_DECL(TestAsset)

  virtual bool OnLoaded(Buffer buffer) override
  {
    XM_ASSERT_EQ(buffer.size, sizeof(histogram));
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
  XM_ASSERT_STREQ(rawNameExt, "testasset.testBasic");

  int histogram[256] = {};

  BufferReader reader(buffer);
  while (reader.GetRemainingSize() > 0)
  {
    ++histogram[*reader.ReadBytes(1)];
  }

  data.write(reinterpret_cast<char*>(histogram), sizeof(histogram));
  return true;
}

void EnsureTestAssetExists(FilePath const& path)
{
  if (!File::CheckExists(path))
  {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<uint32_t> distro;

    FileWriter fw;
    FilePath rawPath(File::kRawProto + File::GetRomPath() / path);
    XM_ASSERT_TRUE(fw.Open(rawPath, xr::FileWriter::Mode::Truncate, false));
    for (int i = 0; i < 2560000; ++i)
    {
      auto val = distro(gen);
      fw.Write(&val, sizeof(val), 1);
    }
  }
}

XM_TEST_F(Asset, Basics)
{
  FilePath path("assets/testasset.testBasic");
  EnsureTestAssetExists(path);

  auto testAss = xr::Asset::Manager::Load<TestAsset>(path);

  XM_ASSERT_TRUE(CheckAllMaskBits(testAss->GetFlags(), xr::Asset::LoadingFlag)); // load in progress
  XM_ASSERT_EQ(xr::Asset::Manager::Find<TestAsset>(path), testAss);  // manager has reference and is same

  while (!(testAss->GetFlags() & (xr::Asset::ReadyFlag | xr::Asset::ErrorFlag)))
  {
    xr::Asset::Manager::Update();
  }
  XM_ASSERT_FALSE(CheckAllMaskBits(testAss->GetFlags(), xr::Asset::ErrorFlag));  // loaded successfully

  XM_ASSERT_EQ(testAss->GetRefCount(), 2); // two references: one in manager, one local
  xr::Asset::Manager::UnloadUnused();

  XM_ASSERT_TRUE(CheckAllMaskBits(testAss->GetFlags(), xr::Asset::ReadyFlag)); // still loaded
  auto desc = testAss->GetDescriptor();

  testAss.Reset(nullptr);

  xr::Asset::Manager::UnloadUnused();
  XM_ASSERT_FALSE(xr::Asset::Manager::Find(desc));  // removed
}

XM_TEST_F(Asset, LoadReflected)
{
  FilePath path("assets/testasset.testBasic");
  EnsureTestAssetExists(path);

  auto testAss = xr::Asset::Manager::LoadReflected(path);

  XM_ASSERT_TRUE(CheckAllMaskBits(testAss->GetFlags(), xr::Asset::LoadingFlag)); // load in progress
  XM_ASSERT_TRUE(testAss->Cast<TestAsset>()); // determined correct type
  XM_ASSERT_EQ(xr::Asset::Manager::Find<TestAsset>(path), testAss);  // manager has reference and is same

  while (!(testAss->GetFlags() & (xr::Asset::ReadyFlag | xr::Asset::ErrorFlag)))
  {
    xr::Asset::Manager::Update();
  }
  XM_ASSERT_FALSE(CheckAllMaskBits(testAss->GetFlags(), xr::Asset::ErrorFlag));  // loaded successfully
}

struct DependantTestAsset : public xr::Asset
{
  XR_ASSET_DECL(DependantTestAsset)

  static std::vector<Asset::DescriptorCore> s_order;

  // Inherited via Asset
  virtual bool OnLoaded(Buffer /*buffer*/) override
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

std::vector<xr::Asset::DescriptorCore> DependantTestAsset::s_order;

XR_ASSET_BUILDER_BUILD_SIG(DependantTestAsset)
{
  (void)rawNameExt;
  (void)data;

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

XM_TEST_F(Asset, Dependencies)
{
  // Load 4 assets of the following dependency structure:
  // test -> test1, test2; test1 -> test3; test2 -> test3; test3
  DependantTestAsset::s_order.clear();

  FilePath path("assets/test.testDeps");

  // none of the dependencies exist
  XM_ASSERT_EQ(xr::Asset::Manager::Find<DependantTestAsset>("assets/test1.testDeps"), xr::Asset::Ptr());
  XM_ASSERT_EQ(xr::Asset::Manager::Find<DependantTestAsset>("assets/test2.testDeps"), xr::Asset::Ptr());
  XM_ASSERT_EQ(xr::Asset::Manager::Find<DependantTestAsset>("assets/test3.testDeps"), xr::Asset::Ptr());

  auto testAss = xr::Asset::Manager::Load<DependantTestAsset>(path, 0);

  // dependencies are created - we don't care if they're loading, as this might
  // be done by the time we've looked them up and checked it (since only the
  // data loading is async and the dependencies are processed synchronously).
  auto dep1 = xr::Asset::Manager::Find<DependantTestAsset>("assets/test1.testDeps");
  auto dep2 = xr::Asset::Manager::Find<DependantTestAsset>("assets/test2.testDeps");
  auto dep3 = xr::Asset::Manager::Find<DependantTestAsset>("assets/test3.testDeps");
  XM_ASSERT_NE(dep1, xr::Asset::Ptr());
  XM_ASSERT_NE(dep2, xr::Asset::Ptr());
  XM_ASSERT_NE(dep3, xr::Asset::Ptr());

  while (!(testAss->GetFlags() & (xr::Asset::ReadyFlag | xr::Asset::ErrorFlag)))
  {
    xr::Asset::Manager::Update();
  }

  XM_ASSERT_TRUE(CheckAllMaskBits(testAss->GetFlags(), xr::Asset::ReadyFlag));
  XM_ASSERT_TRUE(CheckAllMaskBits(dep1->GetFlags(), xr::Asset::ReadyFlag));
  XM_ASSERT_TRUE(CheckAllMaskBits(dep2->GetFlags(), xr::Asset::ReadyFlag));
  XM_ASSERT_TRUE(CheckAllMaskBits(dep3->GetFlags(), xr::Asset::ReadyFlag));

  // Dependencies are loaded in the correct order - first the ones without any dependencies,
  // then the ones that depend upon them etc.
  // We can't inspect their loading directly, however we know that the order that their
  // OnLoaded() is called is the order they're popped off the loading queue.
  XM_ASSERT_EQ(DependantTestAsset::s_order[0], dep3->GetDescriptor());
  XM_ASSERT_EQ(DependantTestAsset::s_order[1], dep1->GetDescriptor());
  XM_ASSERT_EQ(DependantTestAsset::s_order[2], dep2->GetDescriptor());
  XM_ASSERT_EQ(DependantTestAsset::s_order[3], testAss->GetDescriptor());
}
#endif
}

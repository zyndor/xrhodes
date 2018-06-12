#include "FileLifeCycleManager.hpp"

#include "gtest/gtest.h"
#include "xr/Asset.hpp"
#include "xr/Hash.hpp"

#include "xr/Image.hpp"
#include "xr/Texture.hpp"
#include "xr/Material.hpp"

#include "xr/Device.hpp"
#include "xr/Gfx.hpp"
#include "xr/ScopeGuard.hpp"
#include "xr/VertexFormats.hpp"
#include "xr/Transforms.hpp"
#include "xr/Matrix.hpp"

#include <numeric>

namespace xr
{
  struct ImageDiff
  {
    size_t countDiffBytes;
    int diffAccum;
    int diffMax;
    int histogram[256];

    size_t countDiffPixels;

    ImageDiff(Image const& lhs, Image const& rhs, Image& out)
    : countDiffBytes(0),
      diffAccum(0),
      diffMax(0),
      histogram{},
      countDiffPixels(0)
    {
      XR_ASSERT(DiffImage, lhs.GetWidth() == rhs.GetWidth());
      XR_ASSERT(DiffImage, lhs.GetHeight() == rhs.GetHeight());
      XR_ASSERT(DiffImage, lhs.GetBytesPerPixel() == rhs.GetBytesPerPixel());
      out.SetSize(lhs.GetWidth(), lhs.GetHeight(), lhs.GetBytesPerPixel());

      auto p0 = lhs.GetPixelData();
      auto p1 = rhs.GetPixelData();
      auto pout = out.GetPixelData();
      auto pEnd = lhs.GetPixelData() + lhs.GetPixelDataSize();
      uint8_t const* lastDiff = nullptr;
      while (p0 != pEnd)
      {
        uint8_t out;
        if (*p0 > *p1)
        {
          out = *p0 - *p1;
        }
        else
        {
          out = *p1 - *p0;
        }

        if (out > 0)
        {
          ++countDiffBytes;
          if (p0 - lastDiff >= lhs.GetBytesPerPixel())
          {
            ++countDiffPixels;
            lastDiff = p0;
          }

          if (out > diffMax)
          {
            diffMax = out;
          }

          diffAccum += out;
        }

        ++histogram[out];

        *pout = out;
        ++p0;
        ++p1;
        ++pout;
      }
    }
  };

  template <typename T>
  static void AssertEq(T const& a, T const& b)
  {
    ASSERT_EQ(a, b);
  }

  class MaterialTests : public ::testing::Test
  {
  public:
    static void SetUpTestCase()
    {}

    static void TearDownTestCase()
    {}

    MaterialTests()
    {
      Asset::Manager::Init(".assets");

      xr::Device::Init();
      xr::Gfx::Init(xr::Device::GetMainWindow());
      xr::Transforms::Init();

      xr::Transforms::Updater().SetOrthographicProjection(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f).
        SetViewerTransform(xr::Matrix(xr::Vector3(.0f, .0f, .0f)));
    }

    ~MaterialTests()
    {
      Asset::Manager::Exit();

      Gfx::Exit();
      Device::Shutdown();
    }

  private:
    FileLifeCycleManager  flcm;
  };

#ifdef ENABLE_ASSET_BUILDING
  TEST_F(MaterialTests, Textures)
  {
    Texture::RegisterSamplerUniform("xruSampler0", 0);

    FilePath path("assets/xrhodes.mtl");

    auto testFlags = Asset::KeepSourceDataFlag | Texture::PointFilterFlag;
    Material::Ptr material = Asset::Manager::Load<Material>(path, testFlags);

    while (!CheckAnyMaskBits(material->GetFlags(), Asset::ReadyFlag | Asset::ErrorFlag))
    {
      Asset::Manager::Update();
    }
    ASSERT_TRUE(CheckAllMaskBits(material->GetFlags(), Asset::ReadyFlag));

    {
      Texture::Ptr texture = material->GetTexture(0);
      ASSERT_EQ(texture->GetWidth(), 128);
      ASSERT_EQ(texture->GetHeight(), 128);
      ASSERT_EQ(texture->GetFlags(), Asset::ReadyFlag | testFlags);
    }

    using PosUvColor = Vertex::Format<
      Vertex::Pos<Vector3>,
      Vertex::UV0<Vector2>,
      Vertex::Color0<Vector3>>;
    auto hFormat = Vertex::Formats::GetHandle<PosUvColor>();

#ifdef XR_PLATFORM_OSX  // TODO: Remove once https://bugreport.apple.com/web/?problemID=36213140 is resolved.
    PosUvColor vboData[4];
    vboData[0].pos = Vector3( -1.0f, -1.0f, .0f );
    vboData[0].uv0 = Vector2(.0f, .0f);
    vboData[0].color0 = Vector3(1.0f, .0f, .0f);

    vboData[1].pos = Vector3( -1.0f, 1.0f, .0f );
    vboData[1].uv0 = Vector2(.0f, 1.0f);
    vboData[1].color0 = Vector3(.0f, 1.0f, .0f);

    vboData[2].pos = Vector3( 1.0f, -1.0f, .0f );
    vboData[2].uv0 = Vector2(1.0f, .0f);
    vboData[2].color0 = Vector3(.0f, 1.0f, 1.0f);

    vboData[3].pos = Vector3( 1.0f, 1.0f, .0f );
    vboData[3].uv0 = Vector2(1.0f, 1.0f);
    vboData[3].color0 = Vector3(1.0f, .0f, 1.0f);
#else
    PosUvColor vboData[] = {
      PosUvColor( Vector3( -1.0f, -1.0f, .0f ), Vector2( .0f, 0.0f ), Vector3( 1.0f, .0f, .0f )),
      PosUvColor( Vector3( -1.0f, 1.0f, .0f ), Vector2(.0f, 1.0f ), Vector3(.0f, 1.0, .0f )),
      PosUvColor( Vector3( 1.0f, -1.0f, .0f ), Vector2(1.0f, .0f ), Vector3(.0f, 1.0f, 1.0f )),
      PosUvColor( Vector3( 1.0f, 1.0f, .0f ), Vector2(1.0f, 1.0f ), Vector3(1.0f, .0f, 1.0f )),
    };
#endif
    auto vbo = Gfx::CreateVertexBuffer(hFormat, { sizeof(vboData), (uint8_t*)vboData });

    Gfx::Clear(Gfx::F_CLEAR_DEPTH);
    material->Apply();
    Gfx::Draw(vbo, Primitive::TriStrip, 0, 4);
    Gfx::Present();

    Image cap;
    cap.SetSize(Gfx::GetWidth(), Gfx::GetHeight(), 3);

    Gfx::ReadFrameBuffer(0, 0, cap.GetWidth(), cap.GetHeight(), Gfx::TextureFormat::RGB8,
      0, cap.GetPixelData());

    //ASSERT_TRUE(cap.Save("capture.tga", true));

    Image truth;
    ASSERT_TRUE(truth.Load("truths/MaterialTest.png"));
    ASSERT_EQ(truth.GetWidth(), cap.GetWidth());
    ASSERT_EQ(truth.GetHeight(), cap.GetHeight());
    ASSERT_EQ(truth.GetBytesPerPixel(), cap.GetBytesPerPixel());
    ASSERT_EQ(truth.GetPixelDataSize(), cap.GetPixelDataSize());

    // NOTE: Byte-by-byte comparison fails on a different OS running on the same
    // machine as the one the ground truth image was generated on, for what seems like
    // differences in texture filtering, so it's safe to assume that it will fail
    // between different GPUs, other platforms etc. Since we're not developing image
    // authoring software and don't want to maintain multiple copies of the ground
    // truth(s), we'll just set thresholds and settle for anything below.
    //ASSERT_EQ(memcmp(truth.GetPixelData(), cap.GetPixelData(), truth.GetPixelDataSize()), 0);
    Image diff;
    ImageDiff id(cap, truth, diff);
    //diff.Save("diff.png", true);

    ASSERT_LT(id.countDiffBytes / float(truth.GetPixelDataSize()), .005f);
    ASSERT_GE(id.countDiffBytes, 0UL);
    if (id.countDiffBytes > 0)
    {
      int diffSignificant = std::accumulate(id.histogram + 16, id.histogram + 256, 0);
      ASSERT_LT(diffSignificant / float(id.countDiffBytes), .275f);
    }

    material.Reset(nullptr);
    Asset::Manager::UnloadUnused();
  }
#endif
}

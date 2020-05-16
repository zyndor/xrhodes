//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/Vertex.hpp"
#include "xr/Device.hpp"

using namespace xr;

namespace
{
class Vertex: public ::testing::Test
{
public:
  static void SetUpTestCase()
  {}

  static void TearDownTestCase()
  {}

  Vertex()
  {
    Device::Init("Vertex");
    Gfx::Init(Device::GetGfxContext());
  }

  ~Vertex()
  {
    Gfx::Shutdown();
    Device::Shutdown();
  }
};

TEST_F(Vertex, Basics)
{
  using VF1 = xr::Vertex::Format<
    xr::Vertex::Pos<Vector3>,
    xr::Vertex::UV0<Vector2>,
    xr::Vertex::Color0<Vector3>>;

  // Construction omitting some subtypes.
#ifdef XR_PLATFORM_OSX  // TODO: Remove once https://bugreport.apple.com/web/?problemID=36213140 is resolved.
  VF1 v1(Vector3::One() * 2.0f);
  v1.uv0 = Vector2(.5f, .5f);
#else
  VF1 v1(Vector3::One() * 2.0f, Vector2(.5f, .5f));
#endif
  static_assert(VF1::kSize == sizeof(v1), "Size mismatch.");
  ASSERT_EQ(v1.uv0.x, .5f);
  ASSERT_EQ(v1.uv0.y, .5f);

  v1 = VF1(Vector3(1.0f, .0f, -10.0f));
  ASSERT_EQ(v1.pos.x, 1.0f);
  ASSERT_EQ(v1.pos.y, .0f);
  ASSERT_EQ(v1.pos.z, -10.0f);
  ASSERT_EQ(v1.uv0.x, .0f);
  ASSERT_EQ(v1.uv0.y, .0f);

  // Assignment of subtype
  using VF2 = xr::Vertex::Format<xr::Vertex::Bitangent, VF1>;
  VF2 v2;
  static_assert(VF2::kSize == sizeof(v2), "Size mismatch.");
  Vector3 testBitangent(.707f, .707f, .0f);
  v2.bitangent = testBitangent;
  v2 = v1;
  ASSERT_EQ(v2.pos.x, v1.pos.x);
  ASSERT_EQ(v2.pos.y, v1.pos.y);
  ASSERT_EQ(v2.pos.z, v1.pos.z);
  ASSERT_EQ(v2.uv0.x, v1.uv0.x);
  ASSERT_EQ(v2.uv0.y, v1.uv0.y);
  ASSERT_EQ(v2.bitangent.x, testBitangent.x);
  ASSERT_EQ(v2.bitangent.y, testBitangent.y);
  ASSERT_EQ(v2.bitangent.z, testBitangent.z);

  // Assignment of single component
  const Vector3 testColor(1.0f, .25f, .25f);
  v2 = xr::Vertex::Color0<Vector3>(testColor);
  ASSERT_EQ(v2.pos.x, v1.pos.x);
  ASSERT_EQ(v2.pos.y, v1.pos.y);
  ASSERT_EQ(v2.pos.z, v1.pos.z);
  ASSERT_EQ(v2.uv0.x, v1.uv0.x);
  ASSERT_EQ(v2.uv0.y, v1.uv0.y);
  ASSERT_EQ(v2.color0.x, testColor.x);
  ASSERT_EQ(v2.color0.y, testColor.y);
  ASSERT_EQ(v2.color0.z, testColor.z);
  ASSERT_EQ(v2.bitangent.x, testBitangent.x);
  ASSERT_EQ(v2.bitangent.y, testBitangent.y);
  ASSERT_EQ(v2.bitangent.z, testBitangent.z);
}

}

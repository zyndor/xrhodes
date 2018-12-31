//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "GfxCommon.hpp"

namespace xr
{
namespace Gfx
{

//==============================================================================
const uint32_t Const::kUniformTypeSize[] =
{
  sizeof(int32_t),
  sizeof(float) * 4,
  sizeof(float) * 3 * 3,
  sizeof(float) * 4 * 4,
  1
};
static_assert(XR_ARRAY_SIZE(Const::kUniformTypeSize) ==
  size_t(UniformType::kCount) + 1,
  "Uniform type count / size definition count must match.");

//==============================================================================
char const *const  Const::kAttributeName[] =
{
  "aPosition",
  "aNormal",
  "aTangent",
  "aBitangent",
  "aColor0",
  "aColor1",
  "aUV0",
  "aUV1",
  "aUV2",
  "aUV3",
  "aUV4",
  "aUV5",
  "aUV6",
  "aUV7",
};
static_assert(XR_ARRAY_SIZE(Const::kAttributeName) ==
  size_t(Attribute::kCount), "Count of attribute types / names must match.");

//==============================================================================
char const *const  Const::kInstanceDataName[] =
{
  "iData0",
  "iData1",
  "iData2",
  "iData3",
};
static_assert(XR_ARRAY_SIZE(Const::kInstanceDataName) ==
  kMaxInstanceData, "Count of attribute types / names must match.");


} // Gfx
}

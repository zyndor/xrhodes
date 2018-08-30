#ifndef XR_QUAD_HPP
#define XR_QUAD_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <stdint.h>

namespace xr
{
namespace Quad
{
namespace Vertex
{
enum Value
{
  NW,
  NE,
  SW,
  SE,
  kCount
};
}

const int kIndexCount = 6;

extern const uint16_t kIndices[kIndexCount];

}
}

#endif //XR_QUADCOMMON_HPP

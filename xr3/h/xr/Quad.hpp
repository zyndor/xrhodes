#ifndef XR_QUAD_HPP
#define XR_QUAD_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
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
  SW,
  NE,
  SE,
  kCount
};
}

const int kIndexCount = 6;

extern const uint16_t kIndices[kIndexCount];

}
}

#endif //XR_QUADCOMMON_HPP

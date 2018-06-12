#ifndef XR_QUADCOMMON_HPP
#define XR_QUADCOMMON_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
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
  SE,
  NE,
  kCount
};
}

const int kIndexCount = 6;

extern const uint16_t kIndices[kIndexCount];

}
}

#endif //XR_QUADCOMMON_HPP

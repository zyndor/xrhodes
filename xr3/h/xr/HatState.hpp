#ifndef XR_HATSTATE_HPP
#define XR_HATSTATE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/utils.hpp"
#include <cstdint>

namespace xr
{
///@brief Information about a device whose state can be described as a
/// combination of four cardinal directions, e.g. a game controller hat.
namespace HatState
{
using Type = uint8_t;

enum Values: Type
{
  Up = XR_MASK_ID(uint8_t, 0),
  Down = XR_MASK_ID(uint8_t, 1),
  Left = XR_MASK_ID(uint8_t, 2),
  Right = XR_MASK_ID(uint8_t, 3),
};

enum { kShift = 4 };

} // HatState
}

#endif //XR_HATSTATE_HPP

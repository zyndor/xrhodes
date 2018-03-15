#ifndef XR_BUTTONSTATE_HPP
#define XR_BUTTONSTATE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <cstdint>

namespace XR
{

//==============================================================================
namespace ButtonState
{
using Type = uint8_t;

enum Value: Type
{
  Up,          // 0x0
  WasPressed, // 0x1
  IsPressed,  // 0x2
  Down,        // 0x3
};
}

} // XR

#endif // XR_BUTTONSTATE_HPP

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

namespace xr
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
  Down,        // 0x3,
  kAbsoluteStateBit  // 0x4
};

///@brief Updates the button state given its binary state.
inline
void SetAbsolute(bool isPressed, Type& button)
{
  button = isPressed ? (button | kAbsoluteStateBit) : (button & ~kAbsoluteStateBit);
}

///@brief Performs per-frame update of a button state.
///@note This reserves the 3rd bit for storing the binary up / down state.
inline
void Poll(Type& button)
{
  button = (button & ~Down) | (button >> 1);
}

} // ButtonState

} // xr

#endif // XR_BUTTONSTATE_HPP

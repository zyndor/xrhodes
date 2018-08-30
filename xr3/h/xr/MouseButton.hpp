#ifndef XR_MOUSEBUTTON_HPP
#define XR_MOUSEBUTTON_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <cstdint>

namespace xr {

//==============================================================================
namespace MouseButton
{
enum  Type
{
  Left,
  Right,
  Middle,
  Extra1,
  Extra2,

  // Mouse wheel states. Note that the SDL implementation does not send button
  // up notifications of these. Either poll for them, or assume that they are
  // momentary (i.e. followed by a button up event).
  WheelUp,
  WheelDown,
  WheelLeft,
  WheelRight,

  kCount,
  Unknown = kCount,

  kFirstWheel = WheelUp // The first value mapped to mouse wheel
};

//==============================================================================
extern const int kNative[kCount];

//==============================================================================
Type TranslateNative(int mb);

} // MouseButton

} // xr

#endif // XR_MOUSEBUTTON_HPP

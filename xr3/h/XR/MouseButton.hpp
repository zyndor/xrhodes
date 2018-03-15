#ifndef XR_MOUSEBUTTON_HPP
#define XR_MOUSEBUTTON_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <cstdint>

namespace XR {

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

  kCount,
  Unknown = kCount
};

//==============================================================================
extern const int kNative[kCount];

//==============================================================================
Type TranslateNative(int mb);

} // MouseButton

} // XR

#endif // XR_MOUSEBUTTON_HPP

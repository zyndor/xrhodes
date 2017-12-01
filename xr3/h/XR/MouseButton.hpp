#ifndef XR_MOUSEBUTTON_HPP
#define XR_MOUSEBUTTON_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

namespace XR {

//==============================================================================
enum  MouseButton
{
  MB_LEFT,
  MB_RIGHT,
  MB_MIDDLE,
  MB_EXTRA1,
  MB_EXTRA2,
  
  kMouseButtonCount,
  MB_UNKNOWN = kMouseButtonCount
};

//==============================================================================
extern const int karMouseButtonNative[kMouseButtonCount];

//==============================================================================
MouseButton TranslateMouseButtonNative(int mb);

} // XR

#endif // XR_MOUSEBUTTON_HPP

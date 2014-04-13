//
// Nuclear Heart Games
// XRhodes
//
// MouseButton.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    17/01/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
#if !defined XR_MOUSEBUTTON_HPP
#define XR_MOUSEBUTTON_HPP

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

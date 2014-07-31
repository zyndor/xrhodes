//
// Nuclear Heart Games
// XRhodes
//
// ButtonState.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    07/06/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_BUTTONSTATE_HPP
#define XR_BUTTONSTATE_HPP

namespace XR
{

//==============================================================================
enum  ButtonState
{
  BUTTON_UP,          // 0x0
  BUTTON_WAS_PRESSED, // 0x1
  BUTTON_IS_PRESSED,  // 0x2
  BUTTON_DOWN,        // 0x3
};

} // XR

#endif // XR_BUTTONSTATE_HPP

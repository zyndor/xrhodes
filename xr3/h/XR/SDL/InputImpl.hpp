//
// Nuclear Heart Games
// XRhodes
//
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
//
// copyright (c) 2011 - 2016. All rights reserved.
//
//==============================================================================
#ifndef XR_INPUTIMPL_HPP
#define XR_INPUTIMPL_HPP

#include <XR/Input.hpp>

namespace XR {

struct  InputImpl
{
  // static
  static InputImpl* s_pInstance;
  
  // data
  uint8                 arKeyState[kKeyCount];
  
  SVector2              mousePosition;
  uint8                 arMouseButtonState[5];
  
  CallbackObject::List  arCallback[Input::kMaxEvents];
};

} // XR

#endif // XR_INPUTIMPL_HPP

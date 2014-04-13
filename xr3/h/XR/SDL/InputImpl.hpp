//
// Nuclear Heart Games
// XRhodes
//
// InputImpl.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    15/01/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
#if !defined XR_INPUTIMPL_HPP
#define XR_INPUTIMPL_HPP

#include  "Input.hpp"

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

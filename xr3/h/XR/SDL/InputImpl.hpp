//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
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
  uint8_t               arKeyState[kKeyCount];
  
  SVector2              mousePosition;
  uint8_t               arMouseButtonState[5];
  
  CallbackObject::List  arCallback[Input::kMaxEvents];
};

} // XR

#endif // XR_INPUTIMPL_HPP

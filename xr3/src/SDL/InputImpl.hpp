#ifndef XR_INPUTIMPL_HPP
#define XR_INPUTIMPL_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Input.hpp"

namespace XR {

struct  InputImpl
{
  // static
  static InputImpl* s_pInstance;

  // data
  uint8_t               arKeyState[kKeyCount];
  
  SVector2              mousePosition;
  uint8_t               arMouseButtonState[5];
  
  CallbackObject::List  arCallback[static_cast<int>(Input::Event::kCount)];

  // general
  inline
  CallbackObject::List& GetCallbacks(Input::Event ev)
  {
    return arCallback[static_cast<int>(ev)];
  }
};

} // XR

#endif // XR_INPUTIMPL_HPP

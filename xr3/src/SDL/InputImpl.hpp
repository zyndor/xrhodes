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
  static InputImpl* s_instance;

  // data
  ButtonState::Type keyStates[kKeyCount];

  SVector2  mousePosition;
  ButtonState::Type mouseButtonStates[5];

  CallbackObject::List  callbacks[static_cast<int>(Input::Event::kCount)];

  // general
  inline
  CallbackObject::List& GetCallbacks(Input::Event ev)
  {
    return callbacks[static_cast<int>(ev)];
  }
};

} // XR

#endif // XR_INPUTIMPL_HPP

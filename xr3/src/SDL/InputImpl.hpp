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
#include "SDL_events.h"

namespace XR {

struct  InputImpl
{
  // static
  static InputImpl* s_instance;

  // data
  ButtonState::Type keyStates[kKeyCount];

  SVector2  mousePosition;
  ButtonState::Type mouseButtonStates[MouseButton::kCount];

  CallbackObject::List  callbacks[static_cast<int>(Input::Event::kCount)];

  // general
  inline
  CallbackObject::List& GetCallbacks(Input::Event ev)
  {
    return callbacks[static_cast<int>(ev)];
  }

  void OnKeyEvent(SDL_KeyboardEvent const& e)
  {
    Input::KeyEvent xe{ TranslateKeyCodeNative(e.keysym.scancode),
      e.state == SDL_PRESSED };
    ButtonState::SetAbsolute(xe.isPressed, keyStates[xe.key]);
    CallbackObject::CallList(GetCallbacks(Input::Event::Key), &xe);
  }

  void OnMouseMotion(SDL_MouseMotionEvent const& e)
  {
    Input::MouseMotionEvent xe{ e.which, e.x, e.y };
    mousePosition.x = xe.x;
    mousePosition.y = xe.y;
    CallbackObject::CallList(GetCallbacks(Input::Event::MouseMotion), &xe);
  }

  void OnMouseAction(SDL_MouseButtonEvent const& e)
  {
    Input::MouseActionEvent xe{ e.which, MouseButton::TranslateNative(e.button),
      e.x, e.y, e.state == SDL_PRESSED };
    ButtonState::SetAbsolute(xe.isPressed, mouseButtonStates[xe.button]);
    mousePosition.x = xe.x;
    mousePosition.y = xe.y;
    CallbackObject::CallList(GetCallbacks(Input::Event::MouseAction), &xe);
  }

  void OnMouseWheel(SDL_MouseWheelEvent const& e)
  {
    // SDL mouse wheel events are treated separately from button presses,
    // not obviously accessible in the polling approach, and not carrying position
    // information. Here we translate them to a button event and use the last
    // known mouse position.
    int index = (1 - e.y) / 2 + 2 * e.x * e.x + (e.x + 1) / 2;
    Input::MouseActionEvent xe{ e.which,
      static_cast<MouseButton::Type>(MouseButton::WheelUp + index),
      mousePosition.x, mousePosition.y, true };
    ButtonState::SetAbsolute(xe.isPressed, mouseButtonStates[xe.button]);
    CallbackObject::CallList(GetCallbacks(Input::Event::MouseAction), &xe);
  }
};

} // XR

#endif // XR_INPUTIMPL_HPP

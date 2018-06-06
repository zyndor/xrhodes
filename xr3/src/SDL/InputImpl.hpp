#ifndef XR_INPUTIMPL_HPP
#define XR_INPUTIMPL_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "GameController.hpp"
#include "xr/Input.hpp"
#include "SDL_events.h"

namespace xr {

struct  InputImpl
{
  // types
  enum { kMaxControllers = 8 };

  // static
  static InputImpl* s_instance;

  static HatState::Type TranslateHat(uint8_t state)
  {
    return ((state & SDL_HAT_UP) ? HatState::Up : 0) |
      ((state & SDL_HAT_DOWN) ? HatState::Down : 0) |
      ((state & SDL_HAT_LEFT) ? HatState::Left : 0) |
      ((state & SDL_HAT_RIGHT) ? HatState::Right : 0);
  }

  // data
  ButtonState::Type keyStates[kKeyCount];

  SVector2  mousePosition;
  ButtonState::Type mouseButtonStates[MouseButton::kCount];

  CallbackObject::List  callbacks[static_cast<int>(Input::Event::kCount)];

  GameController controllers[kMaxControllers];

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

  void OpenController(int32_t id)
  {
    XR_ASSERT(Input, id >= 0);
    if (id < kMaxControllers)
    {
      // Open device.
      auto& controller = controllers[id];
      InstPtr ii = { controller.Open(id), &controller };

      // Register instance id since subsequent events will reference this.
      auto iFind = std::lower_bound(m_controllerInstMap, m_lastInstIndex, ii);
      if (iFind == m_lastInstIndex || iFind->instance != ii.instance)
      {
        std::memmove(iFind + 1, iFind, (m_lastInstIndex - iFind) * sizeof(InstPtr));
        *iFind = ii;
        ++m_lastInstIndex;
      }

      Input::JoyDeviceEvent xe = { static_cast<uint32_t>(id), controllers[id].GetName() };
      CallbackObject::CallList(GetCallbacks(Input::Event::JoyAdded), &xe);
    }
    else
    {
      XR_TRACE(Input, ("Controller %d ignored; a maximum of %d supported", id,
        kMaxControllers));
    }
  }

  GameController* GetController(uint32_t id)
  {
    InstPtr result = { id, nullptr };
    auto iFind = std::lower_bound(m_controllerInstMap, m_lastInstIndex, result);
    if (iFind != m_lastInstIndex && iFind->instance == id)
    {
      result.controller = iFind->controller;
    }
    return result.controller;
  }

  void OnJoyAxis(SDL_JoyAxisEvent const& e)
  {
    Input::JoyAxisEvent xe = { static_cast<uint32_t>(e.which), e.axis,
      GameController::Normalize(e.value) };
    GetController(e.which)->GetAxis(e.axis) = xe.value;
    CallbackObject::CallList(GetCallbacks(Input::Event::JoyAxisMotion), &xe);
  }

  void OnJoyButton(SDL_JoyButtonEvent const& e)
  {
    Input::JoyButtonEvent xe = { static_cast<uint32_t>(e.which), e.button,
      e.state == SDL_PRESSED };
    ButtonState::SetAbsolute(xe.isPressed, GetController(e.which)->GetButton(e.button));
    CallbackObject::CallList(GetCallbacks(Input::Event::JoyButton), &xe);
  }

  void OnJoyHat(SDL_JoyHatEvent const& e)
  {
    Input::JoyHatEvent xe = { static_cast<uint32_t>(e.which), e.hat,
      TranslateHat(e.value) };
    GetController(e.which)->GetHat(e.hat) = xe.state << HatState::kShift;
    CallbackObject::CallList(GetCallbacks(Input::Event::JoyHat), &xe);
  }

  void OnJoyBall(SDL_JoyBallEvent const& e)
  {
    Input::JoyBallEvent xe = { static_cast<uint32_t>(e.which), e.ball,
      { GameController::Normalize(e.xrel),
        GameController::Normalize(e.yrel) }};
    GetController(e.which)->GetBall(e.ball) += xe.value;
    CallbackObject::CallList(GetCallbacks(Input::Event::JoyBallMotion), &xe);
  }

  void CloseController(int32_t id)
  {
    XR_ASSERT(Input, id >= 0 && id < kMaxControllers);
    Input::JoyDeviceEvent xe = { static_cast<uint32_t>(id), controllers[id].GetName() };
    CallbackObject::CallList(GetCallbacks(Input::Event::JoyRemoved), &xe);

    auto iFind = std::lower_bound(m_controllerInstMap, m_lastInstIndex, InstPtr{
      static_cast<uint32_t>(id), nullptr });
    XR_ASSERT(Input, iFind != m_lastInstIndex);
    iFind->controller->Close();
    std::memmove(iFind, iFind + 1, sizeof(InstPtr) * (m_lastInstIndex - iFind - 1));
    --m_lastInstIndex;
  }

private:
  // types
  struct InstPtr
  {
    uint32_t instance;
    GameController* controller;

    bool operator<(InstPtr const& rhs) const
    {
      return instance < rhs.instance;
    }
  };

  // data
  InstPtr m_controllerInstMap[kMaxControllers];
  InstPtr* m_lastInstIndex = m_controllerInstMap;
};

} // XR

#endif // XR_INPUTIMPL_HPP

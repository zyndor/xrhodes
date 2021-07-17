#ifndef XR_INPUTIMPL_HPP
#define XR_INPUTIMPL_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "GameController.hpp"
#include "xr/Input.hpp"
#include "xr/events/SignalBroadcaster.hpp"
#include "xr/types/Singleton.hpp"
#include "xr/types/intutils.hpp"
#include "SDL2/SDL_events.h"
#include <cstring>

namespace xr
{

struct  InputImpl: Singleton<InputImpl>
{
  // types
  enum { kMaxControllers = 8 };

  // static
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

  SignalBroadcaster<Input::KeyData const&>  onKey;
  SignalBroadcaster<Input::MouseActionData const&>  onMouseAction;
  SignalBroadcaster<Input::MouseMotionData const&>  onMouseMotion;
  SignalBroadcaster<Input::TouchActionData const&>  onTouchAction;
  SignalBroadcaster<Input::TouchMotionData const&>  onTouchMotion;
  SignalBroadcaster<Input::JoyDeviceData const&>  onJoyAdded;
  SignalBroadcaster<Input::JoyDeviceData const&>  onJoyRemoved;
  SignalBroadcaster<Input::JoyAxisMotionData const&>  onJoyAxis;
  SignalBroadcaster<Input::JoyButtonPressData const&>  onJoyButton;
  SignalBroadcaster<Input::JoyHatMotionData const&>  onJoyHat;
  SignalBroadcaster<Input::JoyBallMotionData const&>  onJoyBall;

  GameController controllers[kMaxControllers];

  // structors
  InputImpl()
  {
    memset(keyStates, 0x00, sizeof(keyStates));
    memset(mouseButtonStates, 0x00, sizeof(mouseButtonStates));
  }

  // general
  void OnKeyEvent(SDL_KeyboardEvent const& e)
  {
    Input::KeyData xe{ TranslateKeyCodeNative(e.keysym.scancode), e.state == SDL_PRESSED };
    ButtonState::SetAbsolute(xe.isPressed, keyStates[xe.key]);
    onKey.Broadcast(xe);
  }

  void OnMouseAction(SDL_MouseButtonEvent const& e)
  {
    XR_ASSERT(Input, Representable<decltype(Input::MouseActionData::x)>(e.x));
    XR_ASSERT(Input, Representable<decltype(Input::MouseActionData::y)>(e.y));
    Input::MouseActionData xe{ e.which, MouseButton::TranslateNative(e.button),
      int16_t(e.x), int16_t(e.y), e.state == SDL_PRESSED };
    ButtonState::SetAbsolute(xe.isPressed, mouseButtonStates[xe.button]);
    mousePosition.x = xe.x;
    mousePosition.y = xe.y;
    onMouseAction.Broadcast(xe);
  }

  void OnMouseMotion(SDL_MouseMotionEvent const& e)
  {
    XR_ASSERT(Input, Representable<decltype(Input::MouseMotionData::x)>(e.x));
    XR_ASSERT(Input, Representable<decltype(Input::MouseMotionData::y)>(e.y));
    Input::MouseMotionData xe{ e.which, int16_t(e.x), int16_t(e.y) };
    mousePosition.x = xe.x;
    mousePosition.y = xe.y;
    onMouseMotion.Broadcast(xe);
  }

  void OnMouseWheel(SDL_MouseWheelEvent const& e)
  {
    // SDL mouse wheel events are treated separately from button presses,
    // not obviously accessible in the polling approach, and not carrying position
    // information. Here we translate them to a button event and use the last
    // known mouse position.
    int index = (1 - e.y) / 2 + 2 * e.x * e.x + (e.x + 1) / 2;
    Input::MouseActionData xe{ e.which,
      static_cast<MouseButton::Type>(MouseButton::WheelUp + index),
      mousePosition.x, mousePosition.y, true };
    ButtonState::SetAbsolute(xe.isPressed, mouseButtonStates[xe.button]);
    onMouseAction.Broadcast(xe);
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

      Input::JoyDeviceData xe = { static_cast<uint32_t>(id), controllers[id].GetName() };
      onJoyAdded.Broadcast(xe);
    }
    else
    {
      XR_TRACE(Input, ("Controller %d ignored; a maximum of %d supported", id, kMaxControllers));
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
    Input::JoyAxisMotionData xe = { static_cast<uint32_t>(e.which), e.axis,
      GameController::Normalize(e.value) };
    GetController(e.which)->GetAxis(e.axis) = xe.value;
    onJoyAxis.Broadcast(xe);
  }

  void OnJoyButton(SDL_JoyButtonEvent const& e)
  {
    Input::JoyButtonPressData xe = { static_cast<uint32_t>(e.which), e.button,
      e.state == SDL_PRESSED };
    ButtonState::SetAbsolute(xe.isPressed, GetController(e.which)->GetButton(e.button));
    onJoyButton.Broadcast(xe);
  }

  void OnJoyHat(SDL_JoyHatEvent const& e)
  {
    Input::JoyHatMotionData xe = { static_cast<uint32_t>(e.which), e.hat,
      TranslateHat(e.value) };
    GetController(e.which)->GetHat(e.hat) = xe.state << HatState::kShift;
    onJoyHat.Broadcast(xe);
  }

  void OnJoyBall(SDL_JoyBallEvent const& e)
  {
    Input::JoyBallMotionData xe = { static_cast<uint32_t>(e.which), e.ball,
      { GameController::Normalize(e.xrel),
        GameController::Normalize(e.yrel) }};
    GetController(e.which)->GetBall(e.ball) += xe.value;
    onJoyBall.Broadcast(xe);
  }

  void CloseController(int32_t id)
  {
    XR_ASSERT(Input, id >= 0 && id < kMaxControllers);
    Input::JoyDeviceData xe = { static_cast<uint32_t>(id), controllers[id].GetName() };
    onJoyRemoved.Broadcast(xe);

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

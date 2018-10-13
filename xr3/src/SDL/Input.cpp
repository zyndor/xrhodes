//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "InputImpl.hpp"
#include "SDL_touch.h"
#include "SDL.h"

namespace xr
{

//==============================================================================
InputImpl*  InputImpl::s_instance = nullptr;

//==============================================================================
void Input::Init()
{
  XR_ASSERTMSG(Input, InputImpl::s_instance == nullptr, ("Already initialized."));
  InputImpl::s_instance = new InputImpl();

  memset(InputImpl::s_instance->keyStates, 0x00,
    sizeof(InputImpl::s_instance->keyStates));
  memset(InputImpl::s_instance->mouseButtonStates, 0x00,
    sizeof(InputImpl::s_instance->mouseButtonStates));
}

//==============================================================================
void Input::Update()
{
  for (auto& k: InputImpl::s_instance->keyStates)
  {
    ButtonState::Poll(k);
  }

  for (auto& m: InputImpl::s_instance->mouseButtonStates)
  {
    ButtonState::Poll(m);
  }

  for (auto& c: InputImpl::s_instance->controllers)
  {
    c.Update();
  }
}

//==============================================================================
ButtonState::Type Input::GetKeyState(KeyCode k)
{
  XR_ASSERT(Input, k < kKeyCount);
  return InputImpl::s_instance->keyStates[k] & ButtonState::Down;
}

//==============================================================================
SVector2 Input::GetMousePosition()
{
  return InputImpl::s_instance->mousePosition;
}

//==============================================================================
ButtonState::Type Input::GetMouseButtonState(MouseButton::Type mb)
{
  XR_ASSERT(Input, mb < MouseButton::kCount);
  return InputImpl::s_instance->mouseButtonStates[mb] & ButtonState::Down;
}

//==============================================================================
bool Input::IsJoyActive(uint32_t device)
{
  return InputImpl::s_instance->controllers[device].IsActive();
}

//==============================================================================
char const * Input::GetJoyName(uint32_t device)
{
  return InputImpl::s_instance->controllers[device].GetName();
}

//==============================================================================
uint32_t Input::GetJoyNumAxes(uint32_t device)
{
  return InputImpl::s_instance->controllers[device].GetNumAxes();
}

//==============================================================================
uint32_t Input::GetJoyNumButtons(uint32_t device)
{
  return InputImpl::s_instance->controllers[device].GetNumButtons();
}

//==============================================================================
uint32_t Input::GetJoyNumHats(uint32_t device)
{
  return InputImpl::s_instance->controllers[device].GetNumHats();
}

//==============================================================================
uint32_t Input::GetJoyNumBalls(uint32_t device)
{
  return InputImpl::s_instance->controllers[device].GetNumBalls();
}

//==============================================================================
float Input::GetJoyAxis(uint32_t device, uint32_t axis)
{
  return InputImpl::s_instance->controllers[device].GetAxis(axis);
}

//==============================================================================
ButtonState::Type Input::GetJoyButton(uint32_t device, uint32_t button)
{
  return InputImpl::s_instance->controllers[device].GetButton(button) & ButtonState::Down;
}

//==============================================================================
HatState::Type Input::GetJoyHat(uint32_t device, uint32_t hat)
{
  return InputImpl::s_instance->controllers[device].GetHat(hat);
}

//==============================================================================
Vector2 Input::GetJoyBall(uint32_t device, uint32_t ball)
{
  return InputImpl::s_instance->controllers[device].GetBall(ball);
}

//==============================================================================
Signal<void, Input::KeyData const&>& Input::KeySignal()
{
  return InputImpl::s_instance->onKey;
}

//==============================================================================
Signal<void, Input::MouseActionData const&>& Input::MouseActionSignal()
{
  return InputImpl::s_instance->onMouseAction;
}

//==============================================================================
Signal<void, Input::MouseMotionData const&>& Input::MouseMotionSignal()
{
  return InputImpl::s_instance->onMouseMotion;
}

//==============================================================================
Signal<void, Input::TouchActionData const&>& Input::TouchActionSignal()
{
  return InputImpl::s_instance->onTouchAction;
}

//==============================================================================
Signal<void, Input::TouchMotionData const&>& Input::TouchMotionSignal()
{
  return InputImpl::s_instance->onTouchMotion;
}

//==============================================================================
Signal<void, Input::JoyDeviceData const&>& Input::JoyAddedSignal()
{
  return InputImpl::s_instance->onJoyAdded;
}

//==============================================================================
Signal<void, Input::JoyDeviceData const&>& Input::JoyRemovedSignal()
{
  return InputImpl::s_instance->onJoyRemoved;
}

//==============================================================================
Signal<void, Input::JoyAxisMotionData const&>& Input::JoyAxisSignal()
{
  return InputImpl::s_instance->onJoyAxis;
}

//==============================================================================
Signal<void, Input::JoyButtonPressData const&>& Input::JoyButtonSignal()
{
  return InputImpl::s_instance->onJoyButton;
}

//==============================================================================
Signal<void, Input::JoyHatMotionData const&>& Input::JoyHatSignal()
{
  return InputImpl::s_instance->onJoyHat;
}

//==============================================================================
Signal<void, Input::JoyBallMotionData const&>& Input::JoyBallSignal()
{
  return InputImpl::s_instance->onJoyBall;
}

//==============================================================================
void Input::Shutdown()
{
  XR_ASSERTMSG(Input, InputImpl::s_instance != nullptr, ("Not initialized."));
  delete InputImpl::s_instance;
  InputImpl::s_instance = nullptr;
}

} // XR

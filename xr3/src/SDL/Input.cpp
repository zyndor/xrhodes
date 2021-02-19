//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "InputImpl.hpp"
#include "SDL2/SDL_touch.h"

namespace xr
{

//==============================================================================
void Input::Init()
{
  XR_ASSERTMSG(Input, !InputImpl::Instance(), ("Already initialized."));
  new InputImpl();
}

//==============================================================================
void Input::Update()
{
  for (auto& k: InputImpl::Instance()->keyStates)
  {
    ButtonState::Poll(k);
  }

  for (auto& m: InputImpl::Instance()->mouseButtonStates)
  {
    ButtonState::Poll(m);
  }

  for (auto& c: InputImpl::Instance()->controllers)
  {
    c.Update();
  }
}

//==============================================================================
ButtonState::Type Input::GetKeyState(KeyCode k)
{
  XR_ASSERT(Input, k < kKeyCount);
  return InputImpl::Instance()->keyStates[k] & ButtonState::Down;
}

//==============================================================================
SVector2 Input::GetMousePosition()
{
  return InputImpl::Instance()->mousePosition;
}

//==============================================================================
ButtonState::Type Input::GetMouseButtonState(MouseButton::Type mb)
{
  XR_ASSERT(Input, mb < MouseButton::kCount);
  return InputImpl::Instance()->mouseButtonStates[mb] & ButtonState::Down;
}

//==============================================================================
bool Input::IsJoyActive(uint32_t device)
{
  return InputImpl::Instance()->controllers[device].IsActive();
}

//==============================================================================
char const * Input::GetJoyName(uint32_t device)
{
  return InputImpl::Instance()->controllers[device].GetName();
}

//==============================================================================
uint32_t Input::GetJoyNumAxes(uint32_t device)
{
  return InputImpl::Instance()->controllers[device].GetNumAxes();
}

//==============================================================================
uint32_t Input::GetJoyNumButtons(uint32_t device)
{
  return InputImpl::Instance()->controllers[device].GetNumButtons();
}

//==============================================================================
uint32_t Input::GetJoyNumHats(uint32_t device)
{
  return InputImpl::Instance()->controllers[device].GetNumHats();
}

//==============================================================================
uint32_t Input::GetJoyNumBalls(uint32_t device)
{
  return InputImpl::Instance()->controllers[device].GetNumBalls();
}

//==============================================================================
float Input::GetJoyAxis(uint32_t device, uint32_t axis)
{
  return InputImpl::Instance()->controllers[device].GetAxis(axis);
}

//==============================================================================
ButtonState::Type Input::GetJoyButton(uint32_t device, uint32_t button)
{
  return InputImpl::Instance()->controllers[device].GetButton(button) & ButtonState::Down;
}

//==============================================================================
HatState::Type Input::GetJoyHat(uint32_t device, uint32_t hat)
{
  return InputImpl::Instance()->controllers[device].GetHat(hat);
}

//==============================================================================
Vector2 Input::GetJoyBall(uint32_t device, uint32_t ball)
{
  return InputImpl::Instance()->controllers[device].GetBall(ball);
}

//==============================================================================
Signal<void, Input::KeyData const&>& Input::KeySignal()
{
  return InputImpl::Instance()->onKey;
}

//==============================================================================
Signal<void, Input::MouseActionData const&>& Input::MouseActionSignal()
{
  return InputImpl::Instance()->onMouseAction;
}

//==============================================================================
Signal<void, Input::MouseMotionData const&>& Input::MouseMotionSignal()
{
  return InputImpl::Instance()->onMouseMotion;
}

//==============================================================================
Signal<void, Input::TouchActionData const&>& Input::TouchActionSignal()
{
  return InputImpl::Instance()->onTouchAction;
}

//==============================================================================
Signal<void, Input::TouchMotionData const&>& Input::TouchMotionSignal()
{
  return InputImpl::Instance()->onTouchMotion;
}

//==============================================================================
Signal<void, Input::JoyDeviceData const&>& Input::JoyAddedSignal()
{
  return InputImpl::Instance()->onJoyAdded;
}

//==============================================================================
Signal<void, Input::JoyDeviceData const&>& Input::JoyRemovedSignal()
{
  return InputImpl::Instance()->onJoyRemoved;
}

//==============================================================================
Signal<void, Input::JoyAxisMotionData const&>& Input::JoyAxisSignal()
{
  return InputImpl::Instance()->onJoyAxis;
}

//==============================================================================
Signal<void, Input::JoyButtonPressData const&>& Input::JoyButtonSignal()
{
  return InputImpl::Instance()->onJoyButton;
}

//==============================================================================
Signal<void, Input::JoyHatMotionData const&>& Input::JoyHatSignal()
{
  return InputImpl::Instance()->onJoyHat;
}

//==============================================================================
Signal<void, Input::JoyBallMotionData const&>& Input::JoyBallSignal()
{
  return InputImpl::Instance()->onJoyBall;
}

//==============================================================================
void Input::Shutdown()
{
  XR_ASSERTMSG(Input, InputImpl::Instance() != nullptr, ("Not initialized."));
  delete InputImpl::Instance();
}

} // XR

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
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
bool Input::RegisterCallback(Event ev, Callback callback, void* userData)
{
  XR_ASSERT(Input, ev != Event::kCount);
  XR_ASSERT(Input, callback != nullptr);
  auto& cbs = InputImpl::s_instance->GetCallbacks(ev);
  for (auto& i: cbs)
  {
    if (i.callback == callback)
    {
      return false;
    }
  }

  cbs.push_back(CallbackObject(callback, userData));
  return true;
}

//==============================================================================
bool Input::UnregisterCallback(Event ev, Callback callback)
{
  XR_ASSERT(Input, ev != Event::kCount);
  XR_ASSERT(Input, callback != nullptr);
  auto& cbs = InputImpl::s_instance->GetCallbacks(ev);
  for (auto i0 = cbs.begin(), i1 = cbs.end(); i0 != i1; ++i0)
  {
    if (i0->callback == callback)
    {
      cbs.erase(i0);
      return true;
    }
  }
  return false;
}

//==============================================================================
void Input::Shutdown()
{
  XR_ASSERTMSG(Input, InputImpl::s_instance != nullptr, ("Not initialized."));
  delete InputImpl::s_instance;
  InputImpl::s_instance = nullptr;
}

} // XR

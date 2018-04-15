//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "InputImpl.hpp"
#include "SDL_touch.h"
#include "SDL.h"

namespace XR
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
void Input::Exit()
{
  XR_ASSERTMSG(Input, InputImpl::s_instance != nullptr, ("Not initialized."));
  delete InputImpl::s_instance;
  InputImpl::s_instance = nullptr;
}

} // XR

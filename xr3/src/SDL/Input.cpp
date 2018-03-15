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
  int numKeys;
  const uint8_t*  sdlKeys = SDL_GetKeyboardState(&numKeys);
  auto keyStates = InputImpl::s_instance->keyStates;
  for (int i = 0; i < kKeyCount; ++i)
  {
    int k = karKeyCodeNative[i];
    ButtonState::Type currentState = (sdlKeys[k] ? 1 : 0) << 1;
    keyStates[i] = (keyStates[i] >> 1) | currentState;
  }

  int32_t x, y;
  uint32_t  mbState = SDL_GetMouseState(&x, &y);
  InputImpl::s_instance->mousePosition = SVector2(x, y);

  auto mbStates = InputImpl::s_instance->mouseButtonStates;
  for (int i = 0; i < MouseButton::kCount; ++i)
  {
    int b = SDL_BUTTON(MouseButton::kNative[i]);
    ButtonState::Type currentState = (((mbState & b) != 0) << 1);
    mbStates[i] = (mbStates[i] >> 1) | currentState;
  }
}

//==============================================================================
ButtonState::Type Input::GetKeyState(KeyCode k)
{
  XR_ASSERT(Input, k < kKeyCount);
  return InputImpl::s_instance->keyStates[k];
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
  return InputImpl::s_instance->mouseButtonStates[mb];
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

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
InputImpl*  InputImpl::s_pInstance(0);

//==============================================================================
void Input::Init()
{
  XR_ASSERT(Input, InputImpl::s_pInstance == 0);
  InputImpl::s_pInstance = new InputImpl();

  memset(InputImpl::s_pInstance->arKeyState, 0x00,
    sizeof(InputImpl::s_pInstance->arKeyState));
  memset(InputImpl::s_pInstance->mouseButtonStates, 0x00,
    sizeof(InputImpl::s_pInstance->mouseButtonStates));
}

//==============================================================================
void Input::Exit()
{
  XR_ASSERT(Input, InputImpl::s_pInstance != 0);
  delete InputImpl::s_pInstance;
  InputImpl::s_pInstance = 0;
}

//==============================================================================
void Input::Update()
{
  int numKeys;
  const uint8_t*  parKeys(SDL_GetKeyboardState(&numKeys));
  for (int i = 0; i < kKeyCount; ++i)
  {
    int k(karKeyCodeNative[i]);
    InputImpl::s_pInstance->arKeyState[i] =
      (InputImpl::s_pInstance->arKeyState[i] >> 1) |
      ((parKeys[k] ? 1 : 0) << 1);
  }

  int32_t x, y;
  uint32_t  mbState = SDL_GetMouseState(&x, &y);
  InputImpl::s_pInstance->mousePosition = SVector2(x, y);

  auto mbStates = InputImpl::s_pInstance->mouseButtonStates;
  for (int i = 0; i < kMouseButtonCount; ++i)
  {
    int b = SDL_BUTTON(kMouseButtonNative[i]);
    uint8_t currentState = (((mbState & b) != 0) << 1);
    mbStates[i] = (mbStates[i] >> 1) | currentState;
  }
}

//==============================================================================
uint8_t Input::GetKeyState(KeyCode k)
{
  XR_ASSERT(Input, k < kKeyCount);
  return InputImpl::s_pInstance->arKeyState[k];
}

//==============================================================================
SVector2 Input::GetMousePosition()
{
  return InputImpl::s_pInstance->mousePosition;
}

//==============================================================================
uint8_t Input::GetMouseState(MouseButton mb)
{
  XR_ASSERT(Input, mb < kMouseButtonCount);
  return InputImpl::s_pInstance->mouseButtonStates[mb];
}

//==============================================================================
bool Input::RegisterCallback(Event ev, Callback callback, void* userData)
{
  XR_ASSERT(Input, ev != Event::kCount);
  XR_ASSERT(Input, callback != nullptr);
  auto& cbs = InputImpl::s_pInstance->GetCallbacks(ev);
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
  auto& cbs = InputImpl::s_pInstance->GetCallbacks(ev);
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

} // XR

#include <SDL_touch.h>
#include "InputImpl.hpp"

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
  memset(InputImpl::s_pInstance->arMouseButtonState, 0x00,
    sizeof(InputImpl::s_pInstance->arMouseButtonState));
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
  const uint8*  parKeys(SDL_GetKeyboardState(&numKeys));
  for (int i = 0; i < kKeyCount; ++i)
  {
    int k(karKeyCodeNative[i]);
    InputImpl::s_pInstance->arKeyState[i] =
      (InputImpl::s_pInstance->arKeyState[i] >> 1) |
      ((parKeys[k] ? 1 : 0) << 1);
  }
  
  int32 x, y;
  uint32  mbState(SDL_GetMouseState(&x, &y));
  for (int i = 0; i < kMouseButtonCount; ++i)
  {
    uint32  state(InputImpl::s_pInstance->arMouseButtonState[i] >> 1);
    InputImpl::s_pInstance->arMouseButtonState[i] = state | 
      (((mbState & SDL_BUTTON(karMouseButtonNative[i])) != 0) << 1);
  }
}

//==============================================================================
uint8 Input::GetKeyState(KeyCode k)
{
  XR_ASSERT(Input, k < kKeyCount);
  return InputImpl::s_pInstance->arKeyState[k];
}

//==============================================================================
SVector2 Input::GetMousePos()
{
  int x, y;
  SDL_GetMouseState(&x, &y);
  return SVector2(x, y);
}

//==============================================================================
uint8   Input::GetMouseState(MouseButton mb)
{
  XR_ASSERT(Input, mb < kMouseButtonCount);
  return InputImpl::s_pInstance->arMouseButtonState[mb];
}

//==============================================================================
bool Input::RegisterCallback( Event ev, Callback pCb, void* pData )
{
  XR_ASSERT(Input, ev < kMaxEvents);
  XR_ASSERT(Input, pCb != 0);
  for (CallbackObject::List::iterator i0(InputImpl::s_pInstance->arCallback[ev].begin()),
    i1(InputImpl::s_pInstance->arCallback[ev].end()); i0 != i1; ++i0)
  {
    if (i0->pCb == pCb)
    {
      return false;
    }
  }

  InputImpl::s_pInstance->arCallback[ev].push_back(CallbackObject(pCb, pData));
  return true;
}

//==============================================================================
bool Input::UnregisterCallback( Event ev, Callback pCb )
{
  XR_ASSERT(Input, ev < kMaxEvents);
  XR_ASSERT(Input, pCb != 0);
  for (CallbackObject::List::iterator i0(InputImpl::s_pInstance->arCallback[ev].begin()),
    i1(InputImpl::s_pInstance->arCallback[ev].end()); i0 != i1; ++i0)
  {
    if (i0->pCb == pCb)
    {
      InputImpl::s_pInstance->arCallback[ev].erase(i0);
      return true;
    }
  }
  return false;
}

} // XR

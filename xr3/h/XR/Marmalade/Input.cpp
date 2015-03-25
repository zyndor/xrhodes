//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include <s3eKeyboard.h>
#include <s3ePointer.h>
#include "Input.hpp"
//#include "S3eInputDevice.hpp"

namespace XR
{

//==============================================================================
struct  InputImpl
{
  //int32                 numDevices;
  //Input::Device**       parDevices;
  uint8                 arKeyState[kKeyCount];
  uint8                 arMouseButtonState[kMouseButtonCount];
  CallbackObject::List  arCallback[Input::kMaxEvents];
};

//==============================================================================
static InputImpl*  s_pInput(0);

//==============================================================================
static int32  OnKeyAction(void* pSystem, void* pUser)
{
  s3eKeyboardEvent*  pEvent(static_cast<s3eKeyboardEvent*>(pSystem));
  Input::KeyEvent e =
  {
    TranslateKeyCodeNative(pEvent->m_Key),
    pEvent->m_Pressed != 0
  };
  CallbackObject::CallList(s_pInput->arCallback[Input::EV_KEY], &e);
  return 0;
}

//==============================================================================
static int32  OnMouseAction(void* pSystem, void* pUser)
{
  s3ePointerEvent*  pEvent(static_cast<s3ePointerEvent*>(pSystem));
  Input::MouseActionEvent e =
  {
    0,  // default device
    TranslateMouseButtonNative(pEvent->m_Button),
    pEvent->m_x,
    pEvent->m_y,
    pEvent->m_Pressed != 0
  };
  CallbackObject::CallList(s_pInput->arCallback[Input::EV_MOUSE_ACTION], &e);
  return 0;
}

//==============================================================================
static int32  OnMouseMotion(void* pSystem, void* pUser)
{
  s3ePointerMotionEvent*  pEvent(static_cast<s3ePointerMotionEvent*>(pSystem));
  Input::MouseMotionEvent e =
  {
    0,  // default device
    pEvent->m_x,
    pEvent->m_y,
  };
  CallbackObject::CallList(s_pInput->arCallback[Input::EV_MOUSE_MOTION], &e);
  return 0;
}

//==============================================================================
static int32  OnTouchAction(void* pSystem, void* pUser)
{
  s3ePointerTouchEvent*  pEvent(static_cast<s3ePointerTouchEvent*>(pSystem));
  Input::TouchActionEvent e =
  {
    0,  // default device
    pEvent->m_TouchID,
    pEvent->m_x,
    pEvent->m_y,
    pEvent->m_Pressed != 0
  };
  CallbackObject::CallList(s_pInput->arCallback[Input::EV_TOUCH_ACTION], &e);
  return 0;
}

//==============================================================================
static int32  OnTouchMotion(void* pSystem, void* pUser)
{
  s3ePointerTouchMotionEvent*  pEvent(static_cast<s3ePointerTouchMotionEvent*>(pSystem));
  Input::TouchMotionEvent e =
  {
    0,  // default device
    pEvent->m_TouchID,
    pEvent->m_x,
    pEvent->m_y
  };
  CallbackObject::CallList(s_pInput->arCallback[Input::EV_TOUCH_MOTION], &e);
  return 0;
}

//==============================================================================
void Input::Init()
{
  XR_ASSERTMSG(Input, s_pInput == 0, ("Already initialised!"));
  s_pInput = new InputImpl();

  //s_pInput->numDevices = 0; // very much a stub
  memset(s_pInput->arKeyState, 0x00, sizeof(s_pInput->arKeyState));
  memset(s_pInput->arMouseButtonState, 0x00, sizeof(s_pInput->arMouseButtonState));

  s3eKeyboardRegister(S3E_KEYBOARD_KEY_EVENT, OnKeyAction,
    &s_pInput->arCallback[EV_KEY]);
  s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, OnMouseAction,
    &s_pInput->arCallback[EV_MOUSE_ACTION]);
  s3ePointerRegister(S3E_POINTER_MOTION_EVENT, OnMouseMotion,
    &s_pInput->arCallback[EV_MOUSE_MOTION]);
  s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, OnTouchAction,
    &s_pInput->arCallback[EV_TOUCH_ACTION]);
  s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, OnTouchMotion,
    &s_pInput->arCallback[EV_TOUCH_MOTION]);
}

//==============================================================================
void Input::Exit()
{
  XR_ASSERTMSG(Input, s_pInput != 0, ("Already initialised!"));

  s3eKeyboardUnRegister(S3E_KEYBOARD_KEY_EVENT, OnKeyAction);
  s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, OnMouseAction);
  s3ePointerUnRegister(S3E_POINTER_MOTION_EVENT, OnMouseMotion);
  s3ePointerUnRegister(S3E_POINTER_TOUCH_EVENT, OnTouchAction);
  s3ePointerUnRegister(S3E_POINTER_TOUCH_MOTION_EVENT, OnTouchMotion);

  //s_pInput->numDevices = 0;
  delete s_pInput;
  s_pInput = 0;
}

//==============================================================================
void Input::Update()
{
  s3eKeyboardUpdate();
  for(int i = 0; i < kKeyCount; ++i)
  {
    int state(s3eKeyboardGetState(static_cast<s3eKey>(karKeyCodeNative[i])));
    s_pInput->arKeyState[i] = (s_pInput->arKeyState[i] >> 1) |
      (((state & (S3E_KEY_STATE_DOWN | S3E_KEY_STATE_PRESSED)) != 0) << 1);
  }

  s3ePointerUpdate();
  for(int i = 0; i < kMouseButtonCount; ++i)
  {
    int state(s3ePointerGetState(static_cast<s3ePointerButton>(i)));
    s_pInput->arMouseButtonState[i] = (s_pInput->arMouseButtonState[i] >> 1) |
      ((state & S3E_POINTER_STATE_DOWN) << 1);
  }
}

//==============================================================================
uint8 Input::GetKeyState(KeyCode k)
{
  XR_ASSERT(Input, k < kKeyCount);
  return s_pInput->arKeyState[k];
}

//==============================================================================
SVector2 Input::GetMousePos()
{
  return SVector2(s3ePointerGetX(), s3ePointerGetY());
}

//==============================================================================
uint8   Input::GetMouseState(MouseButton mb)
{
  XR_ASSERT(Input, mb < kMouseButtonCount);
  return s_pInput->arMouseButtonState[mb];
}

//==============================================================================
bool Input::RegisterCallback( Event ev, Callback pCb, void* pData )
{
  XR_ASSERT(Input, ev < kMaxEvents);
  XR_ASSERT(Input, pCb != 0);
  for(CallbackObject::List::iterator i0(s_pInput->arCallback[ev].begin()),
    i1(s_pInput->arCallback[ev].end()); i0 != i1; ++i0)
  {
    if(i0->pCb == pCb)
    {
      return false;
    }
  }

  s_pInput->arCallback[ev].push_back(CallbackObject(pCb, pData));
  return true;
}

//==============================================================================
bool Input::UnregisterCallback( Event ev, Callback pCb )
{
  XR_ASSERT(Input, ev < kMaxEvents);
  XR_ASSERT(Input, pCb != 0);
  for(CallbackObject::List::iterator i0(s_pInput->arCallback[ev].begin()),
    i1(s_pInput->arCallback[ev].end()); i0 != i1; ++i0)
  {
    if(i0->pCb == pCb)
    {
      s_pInput->arCallback[ev].erase(i0);
      return true;
    }
  }
  return false;
}

} // XR

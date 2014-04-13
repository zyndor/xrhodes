//
// Nuclear Heart Games
// XRhodes
//
// Input.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    03/10/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_INPUT_HPP
#define XR_INPUT_HPP

#include "vectypes.hpp"
#include "Callback.hpp"
#include "KeyCode.hpp"
#include "MouseButton.hpp"

namespace XR
{

//==============================================================================
class Input
{
  XR_NONOBJECT_DECL(Input)
  
public:
  // types
  enum  Event
  {
    EV_KEY,
    EV_MOUSE_ACTION,
    EV_MOUSE_MOTION,
    EV_TOUCH_ACTION,
    EV_TOUCH_MOTION,
    kMaxEvents
  };
  
  enum  ButtonState
  {
    // 0x0 - !(BS_WAS_PRESSED | ISDOWN) - not pressed
    BS_WAS_PRESSED = 0x1, // just released
    BS_IS_PRESSED = 0x2,  // just pressed
    // 0x3 - BS_WAS_PRESSED | BS_IS_PRESSED - held
  };
  
  struct  KeyEvent
  {
    KeyCode k;
    bool    state;
  };
  
  struct  PointerEvent 
  {
    int     id;
    int16   x0;
    int16   y0;
    int16   x;
    int16   y;
    bool    isPressed;
    uint64  ustPressed;
  };
  
  struct  MouseActionEvent
  {
    int     device;
    int     button;
    int16   x;
    int16   y;
    bool    isPressed;
  };

  struct  MouseMotionEvent
  {
    int     device;
    int16   x;
    int16   y;
  };
  
  struct  TouchActionEvent
  {
    int     device;
    int     touch;
    int     x;
    int     y;
    bool    isPressed;
  };
  
  struct  TouchMotionEvent
  {
    int     device;
    int     touch;
    int     x;
    int     y;
  };
  
  // static
  static const int kMaxPointer = 20;
  
  static void     Init();
  static void     Exit();
  
  static void     Update();
  
  // new
  static uint8    GetKeyState(KeyCode k);
  
  static SVector2 GetMousePos();
  static uint8    GetMouseState(MouseButton mb);
  
  static bool     RegisterCallback(Event ev, Callback pCb, void* pData);
  static bool     UnregisterCallback(Event ev, Callback pCb);
};

} // XR

#endif // XR_INPUT_HPP
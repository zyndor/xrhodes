//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_INPUT_HPP
#define XR_INPUT_HPP

#include <XR/fundamentals.hpp>
#include "SVector2.hpp"
#include "Callback.hpp"
#include "KeyCode.hpp"
#include "MouseButton.hpp"
#include "ButtonState.hpp"

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
  
  struct  KeyEvent
  {
    KeyCode k;
    bool    state;
  };
  
  struct  PointerEvent 
  {
    int       id;
    int16_t   x0;
    int16_t   y0;
    int16_t   x;
    int16_t   y;
    bool      isPressed;
    uint64_t  ustPressed;
  };
  
  struct  MouseActionEvent
  {
    int     device;
    int     button;
    int16_t x;
    int16_t y;
    bool    isPressed;
  };

  struct  MouseMotionEvent
  {
    int     device;
    int16_t x;
    int16_t y;
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
  static uint8_t  GetKeyState(KeyCode k);
  
  static SVector2 GetMousePos();
  static uint8_t  GetMouseState(MouseButton mb);
  
  static bool     RegisterCallback(Event ev, Callback pCb, void* pData);
  static bool     UnregisterCallback(Event ev, Callback pCb);
};

} // XR

#endif // XR_INPUT_HPP
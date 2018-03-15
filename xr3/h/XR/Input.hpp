#ifndef XR_INPUT_HPP
#define XR_INPUT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "SVector2.hpp"
#include "KeyCode.hpp"
#include "MouseButton.hpp"
#include "ButtonState.hpp"
#include "XR/fundamentals.hpp"
#include "XR/Callback.hpp"

namespace XR
{

//==============================================================================
class Input
{
  XR_NONOBJECT_DECL(Input)

public:
  // types
  enum class Event
  {
    Key,  // Key pressed or released
    MouseAction,  // Mouse button pressed or released
    MouseMotion,  // Mouse moved
    TouchAction,  // Touch pressed or released
    TouchMotion,  // Touch motion between press and release
    kCount
  };

  struct  KeyEvent
  {
    KeyCode key;
    bool    state;
  };

  struct  MouseActionEvent
  {
    uint32_t  device;
    int       button;
    int16_t   x;
    int16_t   y;
    bool      isPressed;
  };

  struct  MouseMotionEvent
  {
    uint32_t  device;
    int16_t   x;
    int16_t   y;
  };

  struct  TouchActionEvent
  {
    uint32_t  device;
    uint32_t  touch;
    int       x;
    int       y;
    bool      isPressed;
  };

  struct  TouchMotionEvent
  {
    uint32_t  device;
    uint32_t  touch;
    int       x;
    int       y;
  };

  // static
  static const int kMaxPointer = 20;

  static void     Init();
  static void     Exit();

  static void     Update();

  // new
  static ButtonState::Type  GetKeyState(KeyCode k);

  static SVector2 GetMousePosition();
  static ButtonState::Type  GetMouseState(MouseButton mb);

  static bool     RegisterCallback(Event ev, Callback pCb, void* pData);
  static bool     UnregisterCallback(Event ev, Callback pCb);
};

} // XR

#endif // XR_INPUT_HPP

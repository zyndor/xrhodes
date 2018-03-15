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
///@brief Provides an abstraction for input handling with two approaches:
/// 1, events based. Register you callbacks and get notifications of key /
/// mouse / touch state changes. For buttons this only means positive / negative
/// edge information.
/// 2, polling. You call Update() and get button states and pointer positions.
/// For buttons this means their current state, i.e. whether it was up, just
/// pressed, held, or just released.
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
    bool    isPressed;
  };

  struct  MouseActionEvent
  {
    uint32_t  device;
    int16_t   x;
    int16_t   y;
    MouseButton::Type button;
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

  ///@brief Initializes the input subsystem. Requires Device::Init() to
  /// have been called for correct functioning.
  static void     Init();

  ///@brief Polls the system for input changes to return from the Get*
  /// functions.
  ///@note Device::YieldOS() will need to be called beforehand.
  static void     Update();

  ///@brief Gets the state of the given key @a k.
  static ButtonState::Type  GetKeyState(KeyCode k);

  ///@return The current position of the main mouse.
  static SVector2 GetMousePosition();

  ///@return The current state of the main mouse's given button @a mb.
  static ButtonState::Type  GetMouseButtonState(MouseButton::Type mb);

  ///@brief Registers a callback for the given input event @a ev, with the
  /// given @a userData.
  ///@return The success of the operation -- will fail if the @a callback has
  /// already been registered.
  static bool RegisterCallback(Event ev, Callback callback, void* userData);

  ///@brief Removes a registration of the given @a callback for the given
  /// event @a ev.
  ///@return The success of the operation. Will fail if the @a callback has
  /// not been registered.
  static bool UnregisterCallback(Event ev, Callback callback);

  static void Exit();
};

} // XR

#endif // XR_INPUT_HPP

#ifndef XR_INPUT_HPP
#define XR_INPUT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "KeyCode.hpp"
#include "MouseButton.hpp"
#include "ButtonState.hpp"
#include "HatState.hpp"
#include "SVector2.hpp"
#include "Vector2.hpp"
#include "xr/Signal.hpp"
#include "xr/fundamentals.hpp"
#include "xr/Callback.hpp"

namespace xr
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
  ///@brief Information about a keystroke event.
  struct  KeyData
  {
    KeyCode key;
    bool    isPressed;
  };

  ///@brief Information about a mouse or pointer button press event.
  struct  MouseActionData
  {
    uint32_t  device;
    MouseButton::Type button;
    int32_t   x;
    int32_t   y;
    bool      isPressed;
  };

  ///@brief Information about a mouse or pointer motion event.
  struct  MouseMotionData
  {
    uint32_t  device;
    int32_t   x;
    int32_t   y;
  };

  ///@brief Information about a touch press / release event.
  struct  TouchActionData
  {
    uint32_t  device;
    uint32_t  touch;
    int       x;
    int       y;
    bool      isPressed;
  };

  ///@brief Information about a touch motion event.
  struct  TouchMotionData
  {
    uint32_t  device;
    uint32_t  touch;
    int       x;
    int       y;
  };

  ///@brief Information about a game controller being added or removed.
  struct JoyDeviceData
  {
    uint32_t device;
    const char* name;
  };

  ///@brief Information about a game controller axis motion event.
  struct JoyAxisMotionData
  {
    uint32_t device;
    uint8_t axis;
    float value;
  };

  ///@brief Information about a game controller button press event.
  struct JoyButtonPressData
  {
    uint32_t device;
    uint8_t button;
    bool isPressed;
  };

  ///@brief Information about a game controller hat motion.
  struct JoyHatMotionData
  {
    uint32_t device;
    uint8_t hat;
    uint8_t state;
  };

  ///@brief Information about a game controller ball motion.
  struct JoyBallMotionData
  {
    uint32_t device;
    uint8_t ball;
    Vector2 value;  // relative motion
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

  ///@return Whether there is an active controller with the given id, @a device.
  static bool IsJoyActive(uint32_t device);

  ///@return The name of the given controller.
  static char const* GetJoyName(uint32_t device);

  ///@return The number of axes for the given controller, i.e. the maximum id
  /// GetJoyAxis() can be queried with.
  static uint32_t GetJoyNumAxes(uint32_t device);

  ///@return The number of buttons for the given controller, i.e. the maximum id
  /// GetJoyButton() can be queried with.
  static uint32_t GetJoyNumButtons(uint32_t device);

  ///@return The number of hats for the given controller, i.e. the maximum id
  /// GetJoyHat() can be queried with.
  static uint32_t GetJoyNumHats(uint32_t device);

  ///@return The number of balls for the given controller, i.e. the maximum id
  /// GetJoyBall() can be queried with.
  static uint32_t GetJoyNumBalls(uint32_t device);

  ///@return The current absolute state of the given @a axis of the @a device.
  ///@note The device must be active.
  static float GetJoyAxis(uint32_t device, uint32_t axis);

  ///@return The current absolute state of the given @a button of the @a device.
  ///@note The device must be active.
  static ButtonState::Type GetJoyButton(uint32_t device, uint32_t button);

  ///@return The current absolute state of the given @a hat of the @a device.
  ///@note The device must be active.
  static HatState::Type GetJoyHat(uint32_t device, uint32_t hat);

  ///@return The current absolute position of the given @a ball of the @a device.
  ///@note The device must be active.
  static Vector2 GetJoyBall(uint32_t device, uint32_t ball);

  static Signal<void, KeyData const&>& KeySignal();
  static Signal<void, MouseActionData const&>& MouseActionSignal();
  static Signal<void, MouseMotionData const&>& MouseMotionSignal();
  static Signal<void, TouchActionData const&>& TouchActionSignal();
  static Signal<void, TouchMotionData const&>& TouchMotionSignal();
  static Signal<void, JoyDeviceData const&>& JoyAddedSignal();
  static Signal<void, JoyDeviceData const&>& JoyRemovedSignal();
  static Signal<void, JoyAxisMotionData const&>& JoyAxisSignal();
  static Signal<void, JoyButtonPressData const&>& JoyButtonSignal();
  static Signal<void, JoyHatMotionData const&>& JoyHatSignal();
  static Signal<void, JoyBallMotionData const&>& JoyBallSignal();

  static void Shutdown();
};

} // xr

#endif // XR_INPUT_HPP

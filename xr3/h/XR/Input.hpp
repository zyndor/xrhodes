#ifndef XR_INPUT_HPP
#define XR_INPUT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "KeyCode.hpp"
#include "MouseButton.hpp"
#include "ButtonState.hpp"
#include "HatState.hpp"
#include "SVector2.hpp"
#include "Vector2.hpp"
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
    JoyAdded, // A game controller is connected to the system.
    JoyRemoved, // A game controller is disconnected form the system.
    JoyAxisMotion,  // Motion along one of the given controller's axes.
    JoyButton,  // A game controller button was pressed.
    JoyHat, // A game controller hat's state has changed.
    JoyBallMotion,  // A game controller's ball position has changed.
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
    MouseButton::Type button;
    int32_t   x;
    int32_t   y;
    bool      isPressed;
  };

  struct  MouseMotionEvent
  {
    uint32_t  device;
    int32_t   x;
    int32_t   y;
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

  struct JoyDeviceEvent
  {
    uint32_t device;
    const char* name;
  };

  struct JoyAxisEvent
  {
    uint32_t device;
    uint8_t axis;
    float value;
  };

  struct JoyButtonEvent
  {
    uint32_t device;
    uint8_t button;
    bool isPressed;
  };

  struct JoyHatEvent
  {
    uint32_t device;
    uint8_t hat;
    uint8_t state;
  };

  struct JoyBallEvent
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

#ifndef XR_DEVICE_HPP
#define XR_DEVICE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "File.hpp"
#include "xr/Signal.hpp"
#include "xr/fundamentals.hpp"
#include <string>

namespace xr
{
namespace Gfx
{
//==============================================================================
///@brief Opaque implementation detail class, which Gfx depends on for
/// information about the rendering context and access to a swap chain.
class Context;
}

//==============================================================================
///@brief Device is the abstraction of the platform and environment that an XR
/// based application is running in, and provides configuration, a window to
/// create the Gfx context in, and notifications of application lifecycle
/// events.
class Device
{
  XR_NONOBJECT_DECL(Device)

public:
  // types
  ///@brief Information about a change to the screen resolution / orientation.
  struct ScreenChangeData
  {
    bool      isOrientationChanged;
    uint32_t  width;
    uint32_t  height;
  };

  // general
  static void  Init(char const* title = nullptr);

  ///@brief Gets native window representation of the main window; primarily for
  /// context creation in Gfx. This window only exists between calls to Init()
  /// and Shutdown().
  static Gfx::Context* GetGfxContext();

  static void  SetMainWindowTitle(char const* title);

  ///@return Whether a quit event has been received.
  ///@note This is unrelated to calling Shutdown() and may return true when the
  /// application window is closed or the process is terminated etc.
  static bool  IsQuitting();

  ///@return Whether a pause event has been received without a subsequent
  /// resume event.
  static bool  IsPaused();

  ///@brief Gets a configuration option stored under the given @a groupName
  /// (which may be nullptr), and @a varName.
  ///@return The string value of the configuration option; empty string if
  /// undefined.
  static std::string  GetConfig(const char* groupName, const char* varName);

  ///@brief Attempts to get a configuration option, stored under @a groupName
  /// (which may be nullptr), and @a varName, as an integer.
  ///@return The integer value of the configuration option; @a defaultValue
  /// if undefined or not castable to an integer.
  static int GetConfigInt(const char* groupName, const char* varName, int defaultValue);

  ///@brief Signal about the application being suspended.
  static Signal<void>& SuspendSignal();

  ///@brief Signal about the application being resumed.
  static Signal<void>& ResumeSignal();

  ///@brief Signal about the application being terminated.
  static Signal<void>& QuitSignal();

  ///@brief Signal about the screen / main window resolution changing.
  static Signal<void, ScreenChangeData const&>& ScreenChangeSignal();

  ///@brief Yields to the underlying OS for a duration of @a ms to allow the
  /// processing of application lifecycle and input events.
  static void YieldOS(int32_t ms);

  ///@return Whether a yield is in progress, i.e. called from an Event handler.
  static bool IsYielding();

  ///@brief Shuts down the device abstraction, destroys the main window and
  /// terminates event processing.
  static void Shutdown();
};

} // XR

#endif // XR_DEVICE_HPP

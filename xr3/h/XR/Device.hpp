#ifndef XR_DEVICE_HPP
#define XR_DEVICE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "File.hpp"
#include "XR/Callback.hpp"
#include "XR/fundamentals.hpp"
#include <string>

namespace XR
{

//==============================================================================
class Device
{
  XR_NONOBJECT_DECL(Device)

public:
  // types
  enum Event
  {
    EV_PAUSE, // system data is 0
    EV_RESUME, // system data is 0
    EV_QUIT, // system data is 0
    EV_VOL_UP, // system data is 0
    EV_VOL_DOWN, // system data is 0
    EV_NETWORK_ON, // system data is 0
    EV_NETWORK_OFF, // system data is 0
    EV_SCREEN_CHANGE, // system data is ScreenChangeEvent
    kMaxEvents
  };

  struct ScreenChangeEvent
  {
    bool      isOrientationChanged;
    uint32_t  width;
    uint32_t  height;
  };

  // general
  static void         Init(char const* title = nullptr);

  ///@brief Gets native window representation of the main window; primarily for
  /// context creation in Gfx - obviously, Device and Gfx
  /// implementations still need to match. This window only exists between
  /// calls to Init() and Exit().
  static void*        GetMainWindow();

  static void         SetMainWindowTitle(char const* title);

  static void         Exit();

  static bool         IsQuitting();
  static bool         IsPaused();

  static std::string  GetConfig(const char* pGroup, const char* pId);
  static int          GetConfigInt(const char* pGroup, const char* pId,
                        int defaultValue);

  static bool         RegisterCallback(Event ev, Callback pCb, void* pCbData);
  static bool         UnregisterCallback(Event ev, Callback pCb);

  static void         YieldOS(int32_t ms);
  static bool         IsYielding();
};

} // XR

#endif // XR_DEVICE_HPP

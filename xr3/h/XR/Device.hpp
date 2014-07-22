//
// Nuclear Heart Games
// XRhodes
//
// Device.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    11/09/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_DEVICE_HPP
#define XR_DEVICE_HPP

#include "Callback.hpp"
#include "File.hpp"
#include "Log.hpp"

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
    bool    isOrientationChanged;
    uint32  width;
    uint32  height;
  };
  
  // general
  static void         Init();
  static void         Exit();
  
  static bool         IsQuitting();
  static bool         IsPaused();
  
  static const char*  GetConfig(const char* pGroup, const char* pId);
  static int          GetConfigInt(const char* pGroup, const char* pId,
                        int defaultValue);
  
  static bool         RegisterCallback(Event ev, Callback pCb, void* pCbData);
  static bool         UnregisterCallback(Event ev, Callback pCb);
  
  static void         YieldOS(int32 ms);
  static bool         IsYielding();
};

} // XR

#endif // XR_DEVICE_HPP
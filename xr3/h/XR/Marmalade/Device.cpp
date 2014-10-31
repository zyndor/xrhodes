#include <s3eDevice.h>
#include <s3eConfig.h>
#include <IwGx.h>
#include "Device.hpp"

namespace XR
{

//==============================================================================
struct  DeviceImpl 
{
  // data
  CallbackObject::List  arCallbacks[Device::kMaxEvents];
  CallbackObject::List  arPostponedAdd[Device::kMaxEvents];
  CallbackObject::List  arPostponedRemove[Device::kMaxEvents];
  bool                  isYielding;
};

static DeviceImpl*   s_pDevice(0);

//==============================================================================
static int32 DeviceEventHandler(void* pSystem, void* pUser)
{
  CallbackObject::List*  pList(static_cast<CallbackObject::List*>(pUser));
  CallbackObject::CallList(*pList, pSystem);
  return 0;
}

//==============================================================================
static int32  ScreenChangeEventHandler(void* pSystem, void* pUser)
{
  s3eSurfaceOrientation*  pEvent(static_cast<s3eSurfaceOrientation*>(pSystem));
  Device::ScreenChangeEvent e =
  {
    pEvent->m_OrientationChanged != S3E_FALSE,
    pEvent->m_Width,
    pEvent->m_Height
  };

  return DeviceEventHandler(&e, pUser);
}

//==============================================================================
void Device::Init()
{
  XR_ASSERTMSG(Device, s_pDevice == 0, ("Already initialised!"));

  IwGxInit();

  s_pDevice = new DeviceImpl;
  s_pDevice->isYielding = false;

  s3eDeviceRegister(S3E_DEVICE_PAUSE, DeviceEventHandler,
    &s_pDevice->arCallbacks[EV_PAUSE]);
  s3eDeviceRegister(S3E_DEVICE_UNPAUSE, DeviceEventHandler,
    &s_pDevice->arCallbacks[EV_RESUME]);
  s3eDeviceRegister(S3E_DEVICE_EXIT, DeviceEventHandler,
    &s_pDevice->arCallbacks[EV_QUIT]);
  s3eDeviceRegister(S3E_DEVICE_VOL_UP, DeviceEventHandler,
    &s_pDevice->arCallbacks[EV_VOL_UP]);
  s3eDeviceRegister(S3E_DEVICE_VOL_DOWN, DeviceEventHandler,
    &s_pDevice->arCallbacks[EV_VOL_DOWN]);
  s3eDeviceRegister(S3E_DEVICE_NETWORK_ONLINE, DeviceEventHandler,
    &s_pDevice->arCallbacks[EV_NETWORK_ON]);
  s3eDeviceRegister(S3E_DEVICE_NETWORK_OFFLINE, DeviceEventHandler,
    &s_pDevice->arCallbacks[EV_NETWORK_OFF]);

  s3eSurfaceRegister(S3E_SURFACE_SCREENSIZE, ScreenChangeEventHandler,
    &s_pDevice->arCallbacks[EV_SCREEN_CHANGE]);
}

//==============================================================================
void Device::Exit()
{
  XR_ASSERTMSG(Device, s_pDevice != 0, ("Not initialised."));
  XR_ASSERT(Device, !s_pDevice->isYielding);

  s3eDeviceUnRegister(S3E_DEVICE_PAUSE, DeviceEventHandler);
  s3eDeviceUnRegister(S3E_DEVICE_UNPAUSE, DeviceEventHandler);
  s3eDeviceUnRegister(S3E_DEVICE_EXIT, DeviceEventHandler);
  s3eDeviceUnRegister(S3E_DEVICE_VOL_UP, DeviceEventHandler);
  s3eDeviceUnRegister(S3E_DEVICE_VOL_DOWN, DeviceEventHandler);
  s3eDeviceUnRegister(S3E_DEVICE_NETWORK_ONLINE, DeviceEventHandler);
  s3eDeviceUnRegister(S3E_DEVICE_NETWORK_OFFLINE, DeviceEventHandler);

  s3eSurfaceUnRegister(S3E_SURFACE_SCREENSIZE, ScreenChangeEventHandler);

  delete s_pDevice;
  s_pDevice = 0;

  IwGxTerminate();
}

//==============================================================================
bool Device::IsQuitting()
{
  return s3eDeviceCheckQuitRequest() == S3E_TRUE;
}

//==============================================================================
bool Device::IsPaused()
{
  return s3eDeviceCheckPauseRequest() == S3E_TRUE;
}

//==============================================================================
bool  Device::GetConfig(const char* pGroup, const char* pId, char* pOutValue)
{
  return s3eConfigGetString(pGroup, pId, pOutValue) == S3E_RESULT_SUCCESS;
}

//==============================================================================
int Device::GetConfigInt(const char* pGroup, const char* pId, int defaultValue)
{
  int value;
  if(s3eConfigGetInt(pGroup, pId, &value) == S3E_RESULT_ERROR)
  {
    value = defaultValue;
  }
  return value;
}
  
//==============================================================================
bool Device::RegisterCallback( Event ev, Callback pCb, void* pCbData )
{
  XR_ASSERT(Device, ev < kMaxEvents);
  XR_ASSERT(Device, pCb != 0);

  // check if already added
  for(CallbackObject::List::iterator i0(s_pDevice->arCallbacks[ev].begin()),
    i1(s_pDevice->arCallbacks[ev].end()); i0 != i1; ++i0)
  {
    if(i0->pCb == pCb)
    {
      return false;
    }
  }

  if(s_pDevice->isYielding)
  {
    // check for a postponed add as well
    for(CallbackObject::List::iterator i0(s_pDevice->arPostponedAdd[ev].begin()),
      i1(s_pDevice->arPostponedAdd[ev].end()); i0 != i1; ++i0)
    {
      if(i0->pCb == pCb)
      {
        return false;
      }
    }

    s_pDevice->arPostponedAdd[ev].push_back(CallbackObject(pCb, pCbData));    
  }
  else
  {
    s_pDevice->arCallbacks[ev].push_back(CallbackObject(pCb, pCbData));
  }
  return true;
}

//==============================================================================
bool Device::UnregisterCallback( Event ev, Callback pCb )
{
  XR_ASSERT(Device, ev < kMaxEvents);
  XR_ASSERT(Device, pCb != 0);
  if(s_pDevice->isYielding)
  {
    // if got it, add to remove list
    for(CallbackObject::List::iterator i0(s_pDevice->arCallbacks[ev].begin()),
      i1(s_pDevice->arCallbacks[ev].end()); i0 != i1; ++i0)
    {
      if(i0->pCb == pCb)
      {
        s_pDevice->arPostponedRemove[ev].push_back(*i0);
        return true;
      }
    }

    // if on postponed add list, remove it
    for(CallbackObject::List::iterator i0(s_pDevice->arPostponedAdd[ev].begin()),
      i1(s_pDevice->arPostponedAdd[ev].end()); i0 != i1; ++i0)
    {
      if(i0->pCb == pCb)
      {
        i0 = s_pDevice->arPostponedAdd[ev].erase(i0);
        return true;
      }
    }
  }
  else
  {
    // if got it, remove
    for(CallbackObject::List::iterator i0(s_pDevice->arCallbacks[ev].begin()),
      i1(s_pDevice->arCallbacks[ev].end()); i0 != i1; ++i0)
    {
      if(i0->pCb == pCb)
      {
        s_pDevice->arCallbacks[ev].erase(i0);
        return true;
      }
    }
  }
  return false;
}

//==============================================================================
void  Device::YieldOS(int32 ms)
{
  XR_ASSERT(Device, !s_pDevice->isYielding);
  s_pDevice->isYielding = true;
  s3eDeviceYield(ms);
  s_pDevice->isYielding = false;

  for(int i = 0; i < kMaxEvents; ++i)
  {
    Event e(static_cast<Event>(i));
    CallbackObject::List& lRemove(s_pDevice->arPostponedRemove[i]);
    while(!lRemove.empty())
    {
      bool  result(UnregisterCallback(e, lRemove.front().pCb));
      XR_ASSERT(Device, result);
      lRemove.pop_front();
    }

    CallbackObject::List& lAdd(s_pDevice->arPostponedAdd[i]);
    while(!lAdd.empty())
    {
      CallbackObject& cbo(lAdd.front());
      bool  result(RegisterCallback(e, cbo.pCb, cbo.pCbData));
      XR_ASSERT(Device, result);
      lAdd.pop_front();
    }
  }
}

//==============================================================================
bool  Device::IsYielding()
{
  return s_pDevice->isYielding;
}

} // XR

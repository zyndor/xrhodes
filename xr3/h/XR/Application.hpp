//
// Nuclear Heart Games
// XRhodes
//
// Application.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    09/18/2012
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_APPLICATION_HPP
#define XR_APPLICATION_HPP

#include "State.hpp"
#include "Device.hpp"
#include "File.hpp"
#include "Log.hpp"
#include "Renderer.hpp"
#include "Timer.hpp"
#include "Input.hpp"
#include "Audio.hpp"

namespace XR
{

//==============================================================================
class Application
{
  XR_NONOBJECT_DECL(Application)
public:
  // types
  enum  Event
  {
    EV_UPDATE,
    EV_RENDER,
    EV_SECOND,
    kMaxEvents
  };

  struct  SecondEvent
  {
    int numRenders;
    int numUpdates;
  };

  // static
  static void   Init();
  static void   Exit();
    
  static int32  GetFrameDelayMs();
  static bool   IsRunning();
  
  static void   SetCallback(Event e, Callback pCb, void* pCbData);
  
  static void   BreakUpdate();
  
  static void   SetFrameDelayMs(int32 ms);

  static void   Run();
  
private:
  // static
  static int32           m_frameDelayMs;
  
  static bool            m_isRunning;
  
  static CallbackObject  m_arCallback[kMaxEvents];
  static bool            m_breakUpdate;
};

//==============================================================================
// implementation
//==============================================================================
inline
int32 Application::GetFrameDelayMs()
{
  return m_frameDelayMs;
}

//==============================================================================
inline
bool Application::IsRunning()
{
  return m_isRunning;
}

} // XR

#endif // XR_APPLICATION_HPP
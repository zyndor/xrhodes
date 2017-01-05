//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_APPLICATION_HPP
#define XR_APPLICATION_HPP

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
  struct  SecondEvent
  {
    int numRenders;
    int numUpdates;
  };

  class Runner
  {
  public:
    virtual ~Runner();
    
    virtual void Update() =0;
    virtual void Render() =0;
    virtual void OnSecond(SecondEvent const& se);
  };
  
  // static
  static void   Init();
  static void   Exit();
    
  static int32_t  GetFrameDelayMs();
  static bool   IsRunning();
  
  static void   BreakUpdate();
  
  static void   SetFrameDelayMs(int32_t ms);
  static void   SetFrameCappingMs(int32_t ms);
  
  static void   Run(Runner& r);
  
private:
  // static
  static int32_t  m_frameDelayMs;
  static int32_t  m_frameCappingMs;
  
  static bool     m_isRunning;
  static bool     m_breakUpdate;
};

//==============================================================================
// implementation
//==============================================================================
inline
int32_t Application::GetFrameDelayMs()
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
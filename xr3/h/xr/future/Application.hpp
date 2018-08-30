#ifndef XR_APPLICATION_HPP
#define XR_APPLICATION_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "Device.hpp"
#include "File.hpp"
#include "Log.hpp"
#include "Renderer.hpp"
#include "Timer.hpp"
#include "Input.hpp"
#include "xr/fundamentals.hpp"

namespace xr
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
  static void   Init(char const* title = nullptr);
  static void   Shutdown();
  static void     Init(File::System const& filesys, char const* title = nullptr);

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

} // xr

#endif // XR_APPLICATION_HPP

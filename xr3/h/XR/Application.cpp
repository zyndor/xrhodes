//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Application.hpp"

namespace XR
{

//==============================================================================
Application::Runner::~Runner()
{}

//==============================================================================
void Application::Runner::OnSecond(SecondEvent const&)
{}

//==============================================================================
int32_t  Application::m_frameDelayMs(0);
int32_t  Application::m_frameCappingMs(1000);
bool   Application::m_isRunning(false);
bool   Application::m_breakUpdate(false);  

//==============================================================================
void  Application::Init()
{
  Device::Init();
  File::Init();
  Renderer::Init();
  Input::Init();
  Audio::Init();
}

//==============================================================================
void  Application::Exit()
{
  Hash::DebugClearStringLookup();
  
  Audio::Exit();
  Input::Exit();
  Renderer::Exit();
  File::Exit();
  Device::Exit();
}

//==============================================================================
void Application::SetFrameDelayMs(int32_t ms)
{
  XR_ASSERT(Application, !m_isRunning);
  XR_ASSERT(Application, ms > 0);
  m_frameDelayMs = ms;
  if(m_frameCappingMs < ms)
  {
    m_frameCappingMs = ms;
  }
}

//==============================================================================
void Application::SetFrameCappingMs(int32_t ms)
{
  XR_ASSERT(Application, !m_isRunning);
  XR_ASSERT(Application, ms > 0);
  if(ms < m_frameDelayMs)
  {
    ms = m_frameDelayMs;
    XR_ERROR(("Frame capping can't be set lower than frame delay."));
  }
  m_frameCappingMs = ms;
}

//==============================================================================
void Application::Run(Runner& runner)
{
  XR_ASSERT(Application, !m_isRunning);
  XR_ASSERT(Application, m_frameDelayMs > 0);
  m_isRunning = true;
  
  uint64_t tLast;
  uint64_t tNow = Timer::GetUST();
  int64_t tAccum(m_frameDelayMs);
  int64_t tSecond(0);
  SecondEvent eSec = { 0, 0 };
  
  while (!Device::IsQuitting())
  {    
    // render
    runner.Render();
    
    Renderer::Flush();
    Renderer::Present();
    
    ++eSec.numRenders;
    
    // update time
    tLast = tNow;
    
    tNow = Timer::GetUST();
    int64_t tDelta(tNow - tLast);
    tSecond += tDelta;

    // apply frame capping
    if (tDelta > m_frameCappingMs)
    {
      tDelta = m_frameCappingMs;
    }
    tAccum += tDelta;
    
    // update stats
    if (tSecond >= 1000)
    {
      runner.OnSecond(eSec);
      
      eSec.numUpdates = 0;
      eSec.numRenders = 0;
      
      tSecond -= 1000;
    }
    
    // update
    while (tAccum > 0)
    {
      // update events and input
      Device::YieldOS(0);
      if (Device::IsQuitting())
      {
        break;
      }
      else
      {
        Input::Update();
        
        if (m_breakUpdate)
        {
          tAccum = 0;
          m_breakUpdate = false;
          break;
        }
      }
      
      runner.Update();
      
      tAccum -= m_frameDelayMs;
      ++eSec.numUpdates;
    }
  }
  
  m_isRunning = false;
}

//==============================================================================
void  Application::BreakUpdate()
{
  m_breakUpdate = true;
}

} // XR

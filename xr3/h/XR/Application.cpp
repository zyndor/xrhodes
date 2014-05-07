#include "Application.hpp"

namespace XR
{

//==============================================================================
int32           Application::m_frameDelayMs(0);
bool            Application::m_isRunning(false);
int32           Application::m_lastUpdates(0);
int32           Application::m_lastRenders(0);
CallbackObject  Application::m_arCallback[];
bool            Application::m_breakUpdate(false);  

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
void Application::SetFrameDelayMs( int32 ms )
{
  XR_ASSERT(Application, !m_isRunning);
  XR_ASSERT(Application, ms > 0);
  m_frameDelayMs = ms;
}

//==============================================================================
void Application::Run()
{
  XR_ASSERT(Application, !m_isRunning);
  m_isRunning = true;
  
  uint64 tLast;
  uint64 tNow = Timer::GetUST();
  int64 tAccum(m_frameDelayMs);
  int64 tSecond(0);
  int32 numUpdates(0);
  int32 numRenders(0);
  
  while(!Device::IsQuitting())
  {    
    // render
    m_arCallback[EV_RENDER].Call(0);
    
    Renderer::Flush();
    Renderer::Present();
    
    ++numRenders;
    
    // update time
    tLast = tNow;

    tNow = Timer::GetUST();
    int64 tDelta(tNow - tLast);
    tSecond += tDelta;
    tAccum += tDelta;
    
    // update stats
    if(tSecond >= 1000)
    {
      m_lastUpdates = numUpdates;
      numUpdates = 0;
      
      m_lastRenders = numRenders;
      numRenders = 0;
      
      tSecond -= 1000;
    }
    
    // update
    while(tAccum > 0)
    {
      // update events and input
      Device::YieldOS(0);
      if(Device::IsQuitting())
      {
        break;
      }
      else
      {
        Input::Update();

        if(m_breakUpdate)
        {
          tAccum = 0;
          m_breakUpdate = false;
          break;
        }
      }
      
      m_arCallback[EV_UPDATE].Call(0);

      tAccum -= m_frameDelayMs;
      ++numUpdates;
    }
  }

  m_isRunning = false;
}

//==============================================================================
void  Application::SetCallback(Event e, Callback pCb, void* pCbData)
{
  XR_ASSERT(Application, e < kMaxEvents);
  m_arCallback[e].Set(pCb, pCbData);
}

//==============================================================================
void  Application::BreakUpdate()
{
  m_breakUpdate = true;
}

} // XR

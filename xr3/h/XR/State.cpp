#include "State.hpp"

namespace XR
{

//==============================================================================
State::Manager::Manager()
:  m_stack(),
  m_pCurrent(0),
  m_pExiting(0),
  m_doShutdown(false),
  m_fadeDelay(0),
  m_tFade(0)
{}

//==============================================================================
State::Manager::~Manager()
{
  Clear();
}

//==============================================================================
void  State::Manager::_QuickExit()
{
  XR_ASSERT(StateManager, m_pExiting != 0);
  m_pExiting->Exit();
  if(m_doShutdown)
  {
    m_pExiting->Shutdown();
    m_doShutdown = false;
  }
  m_pExiting = 0;
  m_tFade = 0;
}

//==============================================================================
void  State::Manager::Push(State* pState, int tFadeMs)
{
  XR_ASSERT(StateManager, pState != 0);
  XR_ASSERT(StateManager, tFadeMs >= 0);
  // if another state was fading, quick exit it
  if(m_tFade > 0 && m_pExiting != 0)
  {
    _QuickExit();
  }

  // init new state
  pState->Init();

  // set current state as exiting and if any, start fadeout
  m_pExiting = m_pCurrent;
  if(m_pExiting != 0)
  {
    m_pExiting->FadeOut(tFadeMs);
  }
  else
  {
    pState->Enter();
    pState->FadeIn(tFadeMs);
  }
  m_fadeDelay = tFadeMs;
  m_tFade = tFadeMs;

  // set new current state
  m_pCurrent = pState;
  m_stack.push_back(pState);
}

//==============================================================================
void  State::Manager::Pop(int tFadeMs)
{
  XR_ASSERT(StateManager, tFadeMs >= 0);
  // if another state was fading, quick exit it
  if(m_tFade > 0 && m_pExiting != 0)
  {
    _QuickExit();
  }
    
  // make current state exiting (there has to be one), start fadeout
  XR_ASSERT(StateManager, m_pCurrent != 0);
  m_pExiting = m_pCurrent;
  m_pExiting->FadeOut(tFadeMs);
  m_doShutdown = true;

  m_fadeDelay = tFadeMs;
  m_tFade = tFadeMs;
    
  // set new current state, if any
  XR_ASSERT(StateManager, !m_stack.empty());
  m_stack.pop_back();
  m_pCurrent = m_stack.empty() ? 0 : m_stack.back();
}

//==============================================================================
void  State::Manager::Change(State* pState, int tFadeMs)
{
  XR_ASSERT(StateManager, pState != 0);
  XR_ASSERT(StateManager, tFadeMs >= 0);
  // if another state was fading, quick exit it
  if(m_tFade > 0 && m_pExiting != 0)
  {
    _QuickExit();
  }
    
  // make current state exiting and if any, start fadeout
  XR_ASSERT(StateManager, m_pCurrent != 0);
  m_pExiting = m_pCurrent;
  m_pExiting->FadeOut(tFadeMs);
  m_doShutdown = true;

  m_fadeDelay = tFadeMs;
  m_tFade = tFadeMs;
    
  // set new current state
  XR_ASSERT(StateManager, !m_stack.empty());
  pState->Init();
  m_pCurrent = pState;
  m_stack.back() = pState;
}

//==============================================================================
void  State::Manager::Update(int tFadeMs)
{
  if(m_pExiting != 0)
  {
    // if there's an exiting state, update its fade out.
    m_tFade -= tFadeMs;
    if(m_tFade <= 0)
    {
      // fadeout complete - Exit() and if needed, Shutdown() state
      _QuickExit();
      m_tFade = m_fadeDelay;

      if(m_pCurrent != 0)
      {
        // if there's a new current state, enter it and state its fade in.
        m_pCurrent->Enter();
        m_pCurrent->FadeIn(m_fadeDelay);
      }
    }
    else
    {
      m_pExiting->UpdateFadeOut(tFadeMs);
    }
  }

  if(m_pExiting == 0 && m_pCurrent != 0)
  {
    if(m_tFade > 0)
    {
      m_tFade -= tFadeMs;
      m_pCurrent->UpdateFadeIn(tFadeMs);
    }
    else
    {
      m_pCurrent->Update(tFadeMs);
    }
  }
}

//==============================================================================
void  State::Manager::Render()
{
  if(m_pExiting != 0)
  {
    m_pExiting->Render();
  }
  else if(m_pCurrent != 0)
  {
    m_pCurrent->Render();
  }
}

//==============================================================================
void  State::Manager::Clear()
{
  if(m_stack.size() > 0)
  {
    State*  pLast(m_stack.back());
    pLast->Exit();

    while(!m_stack.empty())
    {
      pLast = m_stack.back();
      pLast->Shutdown();
      m_stack.pop_back();
    }
  }
}
  
//==============================================================================
State::State()
{}

//==============================================================================
State::~State()
{}

//==============================================================================
void  State::FadeIn(int32 tDelta)
{}

//==============================================================================
void  State::UpdateFadeIn(int32 tDelta)
{}

//==============================================================================
void  State::FadeOut(int32 tDelta)
{}

//==============================================================================
void  State::UpdateFadeOut(int32 tDelta)
{}

} // XR

#include "State.hpp"

namespace XR
{

//==============================================================================
State::Manager::Manager()
: m_stack(),
  m_pRequested(0),
  m_pPrevious(0),
  m_shutdownPrevious(false),
  m_tFade(0)
{}

//==============================================================================
State::Manager::~Manager()
{
  Clear();
}

//==============================================================================
void  State::Manager::_ExpirePrevious(int32 tFade)
{
  XR_ASSERT(State::Manager, !m_stack.empty());

  if(m_pPrevious != 0 && m_shutdownPrevious)
  {
    m_pPrevious->Shutdown();
  }

  // exit and hand over last state
  m_stack.back()->Exit(tFade);
  m_pPrevious = m_stack.back();
  m_stack.pop_back();

  m_shutdownPrevious = true;
}

//==============================================================================
void  State::Manager::Push(State* pState, int tFade)
{
  XR_ASSERT(State::Manager, pState != 0);
  XR_ASSERT(State::Manager, tFade >= 0);

  // hold last state for exiting
  m_pPrevious = m_stack.empty() ? 0 : m_stack.back();

  // request new state
  m_pRequested = pState;
  pState->Init();

  m_tFade = tFade;
}

//==============================================================================
void  State::Manager::Pop(int tFade)
{
  XR_ASSERT(State::Manager, tFade >= 0);
  XR_ASSERT(State::Manager, !m_stack.empty());

  // expire previous state
  _ExpirePrevious(tFade);

  // roll on 
  m_pRequested = m_stack.empty() ? 0 : m_stack.back();

  m_tFade = tFade;
}

//==============================================================================
void  State::Manager::Change(State* pState, int tFade)
{
  XR_ASSERT(State::Manager, pState != 0);
  XR_ASSERT(State::Manager, tFade >= 0);
  XR_ASSERT(State::Manager, !m_stack.empty());
  XR_ASSERT(State::Manager, pState != m_stack.back());

  // expire previous state
  _ExpirePrevious(tFade);

  // set pState as requested
  m_pRequested = pState;
  pState->Init();

  m_tFade = tFade;
}

//==============================================================================
void State::Manager::Update(int32 tDelta)
{
  // handle request
  if (m_pRequested != 0)
  {
    // process requested
    m_pRequested->Enter(m_tFade);
    m_stack.push_back(m_pRequested);

    // clear request
    m_pRequested = 0;
  }

  // fading
  if (m_tFade > 0)
  {
    m_tFade -= tDelta;

    // update fade in for state to come, if any
    if (!m_stack.empty())
    {
      m_stack.back()->UpdateFadeIn(tDelta);
    }

    // if there's a previous state fading out, update it
    if (m_pPrevious != 0)
    {
      m_pPrevious->UpdateFadeOut(tDelta);

      if (m_tFade <= 0)
      {
        // stop fading
        if(m_shutdownPrevious)
        {
          // shutdown
          m_pPrevious->Shutdown();
          m_shutdownPrevious = false;
        }
        m_pPrevious = 0;
      }
    }
  }

  // update current state
  if (!m_stack.empty())
  {
    m_stack.back()->Update(tDelta);
  }
}

//==============================================================================
void State::Manager::Render()
{
  if(!m_stack.empty())
  {
    m_stack.back()->Render();
  }

  if (m_pPrevious != 0 && m_tFade > 0)
  {
    m_pPrevious->Render();
  }
}

//==============================================================================
void  State::Manager::Clear()
{
  if(m_stack.size() > 0)
  {
    State*  pLast(m_stack.back());
    pLast->Exit(0);

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
void  State::UpdateFadeIn(int32 tDelta)
{}

//==============================================================================
void  State::UpdateFadeOut(int32 tDelta)
{}

} // XR

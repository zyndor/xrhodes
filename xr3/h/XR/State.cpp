#include "State.hpp"

namespace XR
{

//==============================================================================
State::Manager::Manager()
: m_stack(),
  m_iState(0),
  m_pRequested(0),
  m_pPrevious(0),
  m_shutdownPrevious(false),
  m_tFade(0)
{}

//==============================================================================
State::Manager::~Manager()
{}

//==============================================================================
void  State::Manager::SetStackSize(int size)
{
  XR_ASSERT(State::Manager, size > 0);
  XR_ASSERT(State::Manager, size > m_iState);
  
  StateVector stack(size);
  for (int i = 0; i < size; ++i)
  {
    stack[i] = m_stack[i];
  }
  m_stack.swap(stack);
}

//==============================================================================
void  State::Manager::_ExpireState()
{
  if (m_tFade > 0 && m_pPrevious != 0 && m_shutdownPrevious)
  {
    m_pPrevious->Shutdown();
    m_shutdownPrevious = false;
  }

  m_pPrevious = m_stack[m_iState];
}

//==============================================================================
void  State::Manager::Push(State* pState, int tFade)
{
  XR_ASSERT(State::Manager, pState != 0);
  XR_ASSERT(State::Manager, tFade >= 0);
  XR_ASSERT(State::Manager, pState != m_stack[m_iState]);

  _ExpireState();

  ++m_iState;
  m_pRequested = pState;
  m_tFade = tFade;
  pState->Init();
}

//==============================================================================
void  State::Manager::Pop(int tFade)
{
  XR_ASSERT(State::Manager, tFade >= 0);

  _ExpireState();

  m_shutdownPrevious = true;

  --m_iState;
  m_pRequested = m_iState >= 0 ? m_stack[m_iState] : 0;
  m_tFade = tFade;
}

//==============================================================================
void  State::Manager::Change(State* pState, int tFade)
{
  XR_ASSERT(State::Manager, pState != 0);
  XR_ASSERT(State::Manager, tFade >= 0);
  XR_ASSERT(State::Manager, m_iState >= 0);
  XR_ASSERT(State::Manager, pState != m_stack[m_iState]);

  _ExpireState();

  m_shutdownPrevious = true;

  m_pRequested = pState;
  m_tFade = tFade;
  pState->Init();
}

//==============================================================================
void State::Manager::Update(int32 tDelta)
{
  // handle request
  if (m_pRequested != 0)
  {
    // fadeout
    if (m_pPrevious != 0)
    {
      m_pPrevious->Exit(m_tFade);
    }
  
    // swap down
    m_stack[m_iState] = m_pRequested;
    //if (m_pCurrent != 0) // m_pRequested != 0
    //{
    m_stack[m_iState]->Enter(m_tFade);
    //}

    // clear request
    m_pRequested = 0;
  }

  // fading
  if (m_tFade > 0)
  {
    m_tFade -= tDelta;

    //// if there's a previous state fading, update it
    //if (m_pPrevious != 0)
    //{
    //  m_pPrevious->Update(tDelta);
    //}

    //// stop fading
    //if (m_tFade <= 0)
    //{
    //  m_pCurrent->m_isFadeComplete = true;
    //  if (m_pPrevious != 0)
    //  {
    //    m_pPrevious->m_isFadeComplete = true;
    //  }
    //}
  }

  // update current state
  if (m_stack[m_iState] != 0)
  {
    m_stack[m_iState]->Update(tDelta);
  }
}

//==============================================================================
void State::Manager::Render()
{
  XR_ASSERT(RingBuffer, m_iState >= 0);
  XR_ASSERT(RingBuffer, m_stack[m_iState] != 0);

  m_stack[m_iState]->Render();
  if (m_pPrevious != 0 && m_tFade > 0)
  {
    m_pPrevious->Render();
  }
}

//==============================================================================
State::State()
//: m_isFadeComplete(true)
{}

//==============================================================================
State::~State()
{}

//==============================================================================
void  State::Enter(int32 tFade)
{
  m_tFade = tFade;
}

//==============================================================================
void  State::Exit(int32 tFade)
{
  m_tFade = tFade;
}

//==============================================================================
void  State::Update(int32 tDelta)
{
  if (tDelta > 0)
  {
    m_tFade -= tDelta;
  }
}

} // XR

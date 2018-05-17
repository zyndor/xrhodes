//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/State.hpp"
#include "XR/debug.hpp"

namespace XR
{

//==============================================================================
State::Manager::Manager()
:  m_states()
{}

//==============================================================================
State::Manager::~Manager()
{}

//==============================================================================
void  State::Manager::Push(State& s)
{
  if(!m_states.empty())
  {
    m_states.back()->Exit();
  }
      
  _Push(s);
}
    
//==============================================================================
void  State::Manager::Change(State& s)
{
  XR_ASSERT(State::Manager, !m_states.empty());
  _Pop();
  _Push(s);
}
    
//==============================================================================
void  State::Manager::Pop()
{
  XR_ASSERT(State::Manager, !m_states.empty());
  _Pop();
      
  if(!m_states.empty())
  {
    m_states.back()->Enter();
  }
}
    
//==============================================================================
void  State::Manager::Update(int32_t ms)
{
  if(!m_states.empty())
  {
    Result  result;
    m_states.back()->Update(ms, result);

    switch(result.operation)
    {
    case  Result::PUSH:
      XR_ASSERT(State::Manager, result.state != nullptr);
      Push(*result.state);
      break;

    case  Result::CHANGE:
      XR_ASSERT(State::Manager, result.state != nullptr);
      Change(*result.state);
      break;

    case  Result::POP:
      Pop();
      break;
    
    case  Result::NONE:
      break;
    }
  }
}
    
//==============================================================================
void  State::Manager::Render()
{
  if(!m_states.empty())
  {
    m_states.back()->Render();
  }
}

//==============================================================================
void  State::Manager::Clear()
{
  if(!m_states.empty())
  {
    // Exit() and Shutdown() current state only.
    _Pop();

    // rest of them, if any, only need to be Shutdown().
    while(!m_states.empty())
    {
      m_states.back()->Shutdown();
      m_states.pop_back();
    }
  }
}

//==============================================================================
void  State::Manager::_Push(State& s)
{
  m_states.push_back(&s);
  s.Init();
  s.Enter();
}

//==============================================================================
void  State::Manager::_Pop()
{
  m_states.back()->Exit();
  m_states.back()->Shutdown();
  m_states.pop_back();
}

//==============================================================================
State::State()
{}

//==============================================================================
State::~State()
{}

} // XR

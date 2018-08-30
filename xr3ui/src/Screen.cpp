//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Screen.hpp"
#include "xr/debug.hpp"

namespace xr
{

//==============================================================================
Screen::Screen()
: m_manager(nullptr),
  m_state(State::Hidden),
  m_timer(0),
  m_isRegistered(false),
  m_onBecomeActive(nullptr),
  m_onBecomeActiveData(nullptr)
{}

//==============================================================================
Screen::~Screen()
{}

//==============================================================================
void  Screen::SetOnBecomeActive(BecomeActiveCallback becomeActive, void* userData)
{
  m_onBecomeActive = becomeActive;
  m_onBecomeActiveData = userData;
}

//==============================================================================
void  Screen::Show(ScreenManager& sm, int32_t ms)
{
  XR_ASSERTMSG(Screen, m_state == State::Hiding || m_state == State::Hidden,
    ("Invalid state to Show() screen: %d", m_state));
  m_manager = &sm;
  AddElements();
  ShowImpl(ms);
  if (ms > 0)
  {
    m_state = State::Showing;
    m_timer = ms;
  }
  else
  {
    MakeActive();
  }
}

//==============================================================================
void  Screen::Register()
{
  XR_ASSERT(Screen, !m_isRegistered);
  XR_ASSERT(Screen, m_manager != nullptr);
  m_isRegistered = true;
  RegisterImpl();
}

//==============================================================================
void  Screen::Unregister()
{
  XR_ASSERT(Screen, m_isRegistered);
  XR_ASSERT(Screen, m_manager != nullptr);
  m_isRegistered = false;
  UnregisterImpl();
}

//==============================================================================
void  Screen::Hide(uint32_t ms)
{
  XR_ASSERTMSG(Screen, m_state == State::Showing || m_state == State::Active ||
    (m_state == State::Hiding && ms <= m_timer),
    ("Invalid state to Hide() screen: %d", m_state));
  if (m_isRegistered)
  {
    Unregister();
  }

  HideImpl(ms);
  if (ms > 0)
  {
    m_state = State::Hiding;
    m_timer = ms;
  }
  else
  {
    MakeHidden();
  }
}

//==============================================================================
void  Screen::Update(uint32_t ms)
{
  if (m_timer > 0)
  {
    if (ms >= m_timer)
    {
      switch (m_state)
      {
      case  State::Showing:
        // completed showing - register
        MakeActive();
        break;

      case  State::Hiding:
        // completed hiding - remove elements
        MakeHidden();
        break;

      case  State::Hidden:
      case  State::Active:
        break;
      }
      m_timer = 0;
    }
    else
    {
      m_timer -= ms;
    }
  }
}

//==============================================================================
void  Screen::MakeActive()
{
  m_state = State::Active;
  Register();

  if(m_onBecomeActive)
  {
    (*m_onBecomeActive)(this, m_onBecomeActiveData);
  }
}

//==============================================================================
void  Screen::MakeHidden()
{
  m_state = State::Hidden;
  RemoveElements();
  m_manager = nullptr;
}

} // xr

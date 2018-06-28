//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include "xr/Screen.hpp"

namespace xr
{

//==============================================================================
Screen::Screen()
: m_manager(nullptr),
  m_state(S_HIDDEN),
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
  XR_ASSERTMSG(Screen, m_state > S_ACTIVE,
    ("Invalid state to Show() screen: %d", m_state));
  m_manager = &sm;
  _AddElements();
  _Show(ms);
  if (ms > 0)
  {
    m_state = S_SHOWING;
    m_timer = ms;
  }
  else
  {
    _MakeActive();
  }
}

//==============================================================================
void  Screen::Register()
{
  XR_ASSERT(Screen, !m_isRegistered);
  XR_ASSERT(Screen, m_manager != nullptr);
  m_isRegistered = true;
  _Register();
}

//==============================================================================
void  Screen::Unregister()
{
  XR_ASSERT(Screen, m_isRegistered);
  XR_ASSERT(Screen, m_manager != nullptr);
  m_isRegistered = false;
  _Unregister();
}

//==============================================================================
void  Screen::Hide(uint32_t ms)
{
  XR_ASSERTMSG(Screen, m_state < S_HIDING || (m_state == S_HIDING &&
    ms <= m_timer), ("Invalid state to Hide() screen: %d", m_state));
  if (m_isRegistered)
  {
    Unregister();
  }

  _Hide(ms);
  if (ms > 0)
  {
    m_state = S_HIDING;
    m_timer = ms;
  }
  else
  {
    _MakeHidden();
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
      case  S_SHOWING:
        // completed showing - register
        _MakeActive();
        break;

      case  S_HIDING:
        // completed hiding - remove elements
        _MakeHidden();
        break;

      case  S_HIDDEN:
      case  S_ACTIVE:
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
void  Screen::_MakeActive()
{
  m_state = S_ACTIVE;
  Register();

  if(m_onBecomeActive)
  {
    (*m_onBecomeActive)(this, m_onBecomeActiveData);
  }
}

//==============================================================================
void  Screen::_MakeHidden()
{
  m_state = S_HIDDEN;
  _RemoveElements();
  m_manager = nullptr;
}

} // xr

//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include <XR/Screen.hpp>

namespace XR
{

//==============================================================================
Screen::Screen()
: m_pManager(0),
  m_state(S_HIDDEN),
  m_timer(0),
  m_isRegistered(false),
  m_pOnBecomeActive(0),
  m_pOnBecomeActiveData(0)
{}

//==============================================================================
Screen::~Screen()
{}

//==============================================================================
void  Screen::SetOnBecomeActive(BecomeActiveCallback pCb, void *pData)
{
  m_pOnBecomeActive = pCb;
  m_pOnBecomeActiveData = pData;
}

//==============================================================================
void  Screen::Show(ScreenManager& sm, int32_t ms)
{
  XR_ASSERTMSG(Screen, m_state > S_ACTIVE,
    ("Invalid state to Show() screen: %d", m_state));
  m_pManager = &sm;
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
  XR_ASSERT(Screen, m_pManager != 0);
  m_isRegistered = true;
  _Register();
}

//==============================================================================
void  Screen::Unregister()
{
  XR_ASSERT(Screen, m_isRegistered);
  XR_ASSERT(Screen, m_pManager != 0);
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
void  Screen::Update(int32_t ms)
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
  
  if(m_pOnBecomeActive != 0)
  {
    (*m_pOnBecomeActive)(this, m_pOnBecomeActiveData);
  }
}

//==============================================================================
void  Screen::_MakeHidden()
{
  m_state = S_HIDDEN;
  _RemoveElements();
  m_pManager = 0;
}

} // XR

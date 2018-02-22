//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2016. All rights reserved.
//
//==============================================================================
#include "XR/Cursor.hpp"

namespace XR
{

//==============================================================================
void  Cursor::OnMotion(void* pSystem, void* pUser)
{
  Input::MouseMotionEvent*  pEvent(static_cast<Input::MouseMotionEvent*>(pSystem));

  Cursor*  pCursor(static_cast<Cursor*>(pUser));
  int xDelta(abs(pEvent->x - pCursor->m_image.x));
  int yDelta(abs(pEvent->y - pCursor->m_image.y));
  xDelta *= xDelta;
  yDelta *= yDelta;
  xDelta += yDelta;
  xDelta = static_cast<int32_t>(sqrtf(static_cast<float>(xDelta)));
  int hScr(Renderer::GetScreenHeight());
  if(xDelta >= hScr * pCursor->m_moveTreshold ||
    (pCursor->m_timer > 0 && xDelta >= hScr * pCursor->m_keepAliveTreshold))
  {
    pCursor->Wake();
  }
  pCursor->SetPosition(pEvent->x, pEvent->y);
}

//==============================================================================
void  Cursor::OnAction(void* pSystem, void* pUser)
{
  Input::MouseActionEvent*  pEvent(static_cast<Input::MouseActionEvent*>(pSystem));

  Cursor*  pCursor(static_cast<Cursor*>(pUser));
  pCursor->m_isPressed = pEvent->isPressed;
  if(pEvent->isPressed)
  {
    pCursor->Wake();
  }
}

//==============================================================================
Cursor::Cursor()
: m_isEnabled(false),
  m_image(),
  m_timeOut(INT_FAST32_MAX),
  m_timer(INT_FAST32_MAX),
  m_moveTreshold(.0f),
  m_keepAliveTreshold(.0f),
  m_isPressed(false)
{}

//==============================================================================
Cursor::~Cursor()
{}

//==============================================================================
void  Cursor::SetTimeOut(int ms)
{
  m_timeOut = ms;
}

//==============================================================================
void  Cursor::SetMoveTreshold(float hPcnt)
{
  m_moveTreshold = hPcnt;
}

//==============================================================================
void  Cursor::SetKeepAliveTreshold(float hPcnt)
{
  m_keepAliveTreshold = hPcnt;
}

//==============================================================================
void  Cursor::SetSprite(const Sprite* pSprite)
{
  m_image.SetSprite(pSprite, 1.0f);
}

//==============================================================================
void  Cursor::SetEnabled(bool state)
{
  m_isEnabled = state;
  if(state)
  {
    Input::RegisterCallback(Input::EV_MOUSE_MOTION, OnMotion, this);
    Input::RegisterCallback(Input::EV_MOUSE_ACTION, OnAction, this);
  }
  else
  {
    Input::UnregisterCallback(Input::EV_MOUSE_MOTION, OnMotion);
    Input::UnregisterCallback(Input::EV_MOUSE_ACTION, OnAction);
    m_isPressed = false;
  }
}

//==============================================================================
void  Cursor::SetPosition(int x, int y)
{
  m_image.SetPosition(x, y);
}

//==============================================================================
void  Cursor::Wake()
{
  m_timer = m_timeOut;
  m_image.color.a = 1.0f;
}

//==============================================================================
void  Cursor::Update(int ms)
{
  if(!m_isPressed)
  {
    m_timer -= ms;
    if(m_timer < 0xff)
    {
      m_image.color.a = Saturate(float(m_timer) / 0xff);
    }
  }
}

//==============================================================================
void  Cursor::Render(IUIRenderer& r)
{
  if(m_isEnabled)
  {
    m_image.Render(r);
  }
}

} // XR

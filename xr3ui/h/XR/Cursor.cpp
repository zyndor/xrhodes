#include  "Cursor.hpp"

namespace XR
{

//==============================================================================
void  Cursor::OnMotion(void* pSystem, void* pUser)
{
  XR::Input::MouseMotionEvent*  pEvent(static_cast<XR::Input::MouseMotionEvent*>(pSystem));

  Cursor*  pCursor(static_cast<Cursor*>(pUser));
  int xDelta(abs(pEvent->x - pCursor->m_image.x));
  int yDelta(abs(pEvent->y - pCursor->m_image.y));
  xDelta *= xDelta;
  yDelta *= yDelta;
  xDelta += yDelta;
  xDelta = sqrtf(xDelta);
  int hScr(XR::Renderer::GetScreenHeight());
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
  XR::Input::MouseActionEvent*  pEvent(static_cast<XR::Input::MouseActionEvent*>(pSystem));

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
  m_timeOut(INT_FAST32_MAX),
  m_timer(INT_FAST32_MAX),
  m_moveTreshold(.0f),
  m_keepAliveTreshold(.0f),
  m_image(),
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
void  Cursor::SetSprite(const XR::Sprite* pSprite)
{
  m_image.SetSprite(pSprite, 1.0f);
}

//==============================================================================
void  Cursor::SetEnabled(bool state)
{
  m_isEnabled = state;
  if(state)
  {
    XR::Input::RegisterCallback(XR::Input::EV_MOUSE_MOTION, OnMotion, this);
    XR::Input::RegisterCallback(XR::Input::EV_MOUSE_ACTION, OnAction, this);
  }
  else
  {
    XR::Input::UnregisterCallback(XR::Input::EV_MOUSE_MOTION, OnMotion);
    XR::Input::UnregisterCallback(XR::Input::EV_MOUSE_ACTION, OnAction);
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
      m_image.color.a = XR::Saturate(float(m_timer) / 0xff);
    }
  }
}

//==============================================================================
void  Cursor::Render()
{
  if(m_isEnabled)
  {
    m_image.Render();
  }
}

//==============================================================================
void  Cursor::Render(XR::UIRenderer& r)
{
  if(m_isEnabled)
  {
    m_image.Render(&r);
  }
}

} // XR
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Cursor.hpp"

namespace xr
{

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
    Input::MouseMotionSignal().Connect(MakeCallback(*this, &Cursor::OnMotion));
    Input::MouseActionSignal().Connect(MakeCallback(*this, &Cursor::OnAction));
  }
  else
  {
    Input::MouseMotionSignal().Disconnect(MakeCallback(*this, &Cursor::OnMotion));
    Input::MouseActionSignal().Disconnect(MakeCallback(*this, &Cursor::OnAction));
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

//==============================================================================
void  Cursor::OnMotion(Input::MouseMotionData const& e)
{
  int xDelta(abs(e.x - m_image.x));
  int yDelta(abs(e.y - m_image.y));
  xDelta *= xDelta;
  yDelta *= yDelta;
  xDelta += yDelta;
  xDelta = static_cast<int32_t>(sqrtf(static_cast<float>(xDelta)));
  int hScr = Gfx::GetLogicalHeight();
  if (xDelta >= hScr * m_moveTreshold ||
    (m_timer > 0 && xDelta >= hScr * m_keepAliveTreshold))
  {
    Wake();
  }
  SetPosition(e.x, e.y);
}

//==============================================================================
void  Cursor::OnAction(Input::MouseActionData const& e)
{
  m_isPressed = e.isPressed;
  if (e.isPressed)
  {
    Wake();
  }
}

} // xr

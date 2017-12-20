//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UIHorizontalScrollingLayout.hpp"
#include "XR/UIRenderer.hpp"
#include "XR/Renderer.hpp"

namespace XR
{

//==============================================================================
UIHorizontalScrollingLayout::UIHorizontalScrollingLayout()
: UIHorizontalLayout(),
  m_isTouched(false),
  m_sensitivity(1.0f),
  m_xOffset(0),
  m_lastTouchX(0)
{}

//==============================================================================
UIHorizontalScrollingLayout::~UIHorizontalScrollingLayout()
{}

//==============================================================================
bool UIHorizontalScrollingLayout::OnMouseAction(const Input::MouseActionEvent& e )
{
  if (e.x >= x && e.x < (x + w) &&
    e.y >= y && e.y < (y + h))
  {
    if (e.isPressed)
    {
      if (!m_isTouched)
      {
        m_isTouched = true;
        m_lastTouchX = e.x;
      }
    }
    else if (m_isTouched)
    {
      m_isTouched = false;
    }
  }
  else if (m_isTouched)
  {
    m_isTouched = false;
  }
  return false;
}

//==============================================================================
bool UIHorizontalScrollingLayout::OnMouseMotion(const Input::MouseMotionEvent& e )
{
  if (m_isTouched)
  {
    if (e.x >= x && e.x < (x + w) &&
      e.y >= y && e.y < (y + h))
    {
      int dx(floorf((e.x - m_lastTouchX) * m_sensitivity + .5f));
      if (dx != 0)
      {
        SetXOffset(m_xOffset + dx);
        m_lastTouchX = e.x;
      }
    }
  }
  return false;
}

//==============================================================================
void UIHorizontalScrollingLayout::_AlignElement( UIElement* pElem )
{
  switch (m_align)
  {
  case  AL_LOW:
    pElem->y = y;
    break;

  case  AL_CENTER:
    pElem->y = y + (h - pElem->h) / 2;
    break;

  case  AL_HIGH:
    pElem->y = y + h - pElem->h;
    break;
  }

  if (m_lElements.size() > 0)
  {
    if (m_growDir == GD_POSITIVE)
    {
      pElem->x = m_lElements.back()->x + m_lElements.back()->w + m_spacing;
    }
    else
    {
      pElem->x = m_lElements.back()->x - (pElem->w + m_spacing);
    }
  }
  else
  {
    if (m_growDir == GD_POSITIVE)
    {
      pElem->x = x;
    }
    else
    {
      pElem->x = x + w - pElem->w;
    }

    pElem->x += m_xOffset;
  }

  pElem->OnChange();
}

//==============================================================================
void UIHorizontalScrollingLayout::Render(IUIRenderer& renderer) const
{
  renderer.Render();

  Renderer::SetScissorRect(*this);

  UIHorizontalLayout::Render(renderer);
  renderer.Render();

  Renderer::ClearScissorRect();
}

//==============================================================================
void UIHorizontalScrollingLayout::SetXOffset( int16_t xOffs )
{
  if (xOffs > 0)
  {
    xOffs = 0;
  }
  else
  {
    int ew(CalculateElementsExtent());
    if (ew < w)
    {
      ew = w;
    }

    int minOffs(w - ew);
    if (xOffs < minOffs)
    {
      xOffs = minOffs;
    }
  }

  if (m_xOffset != xOffs)
  {
    m_xOffset = xOffs;
    OnChange();
  }
}

//==============================================================================
void  UIHorizontalScrollingLayout::SetSensitivity(float s)
{
  m_sensitivity = s;
}

} // XR

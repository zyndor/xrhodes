//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/UIHorizontalScrollingLayout.hpp"
#include "xr/UIRenderer.hpp"
#include "xr/Gfx.hpp"
#include "xr/ScissorStack.hpp"

namespace xr
{

//==============================================================================
UIHorizontalScrollingLayout::UIHorizontalScrollingLayout()
: UIHorizontalLayout(),
  m_xOffset(0),
  m_sensitivity(1.0f),
  m_isTouched(false),
  m_lastTouchX(0)
{
  ScissorStack::RequireInit();
}

//==============================================================================
bool UIHorizontalScrollingLayout::OnMouseAction(const Input::MouseActionData& e )
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
bool UIHorizontalScrollingLayout::OnMouseMotion(const Input::MouseMotionData& e )
{
  if (m_isTouched)
  {
    if (e.x >= x && e.x < (x + w) &&
      e.y >= y && e.y < (y + h))
    {
      auto dx = static_cast<int32_t>(floorf((e.x - m_lastTouchX) * m_sensitivity + .5f));
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
void UIHorizontalScrollingLayout::AlignChildImpl( UIElement* elem )
{
  switch (m_align)
  {
  case  AL_LOW:
    elem->y = y;
    break;

  case  AL_CENTER:
    elem->y = y + (h - elem->h) / 2;
    break;

  case  AL_HIGH:
    elem->y = y + h - elem->h;
    break;

  case  AL_NOALIGN:
    break;
  }

  if (m_children.size() > 0)
  {
    if (m_growDir == GD_POSITIVE)
    {
      elem->x = m_children.back()->x + m_children.back()->w + m_spacing;
    }
    else
    {
      elem->x = m_children.back()->x - (elem->w + m_spacing);
    }
  }
  else
  {
    if (m_growDir == GD_POSITIVE)
    {
      elem->x = x;
    }
    else
    {
      elem->x = x + w - elem->w;
    }

    elem->x += m_xOffset;
  }

  elem->OnChange();
}

//==============================================================================
void UIHorizontalScrollingLayout::Render(IUIRenderer& renderer) const
{
  renderer.Render();

  {
    ScissorStack::Scope s3(this, true);

    UIHorizontalLayout::Render(renderer);
    renderer.Render();
  }
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

} // xr

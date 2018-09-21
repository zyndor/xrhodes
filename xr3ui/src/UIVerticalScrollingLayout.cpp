//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/UIVerticalScrollingLayout.hpp"
#include "xr/UIRenderer.hpp"
#include "xr/Gfx.hpp"
#include "xr/ScissorStack.hpp"

namespace xr
{

//==============================================================================
UIVerticalScrollingLayout::UIVerticalScrollingLayout()
: UIVerticalLayout(),
  m_yOffset(0),
  m_sensitivity(1.0f),
  m_isTouched(false),
  m_lastTouchY(0)
{
  ScissorStack::RequireInit();
}

//==============================================================================
bool UIVerticalScrollingLayout::OnMouseAction(const Input::MouseActionData& e )
{
  if (e.x >= x && e.x < (x + w) &&
    e.y >= y && e.y < (y + h))
  {
    if (e.isPressed)
    {
      if (!m_isTouched)
      {
        m_isTouched = true;
        m_lastTouchY = e.y;
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
bool UIVerticalScrollingLayout::OnMouseMotion(const Input::MouseMotionData& e )
{
  if (m_isTouched)
  {
    if (e.x >= x && e.x < (x + w) &&
      e.y >= y && e.y < (y + h))
    {
      auto dy = static_cast<int32_t>(std::floor((e.y - m_lastTouchY) * m_sensitivity + .5f));
      if (dy != 0)
      {
        SetYOffset(m_yOffset + dy);
        m_lastTouchY = e.y;
      }
    }
  }
  return false;
}

//==============================================================================
void UIVerticalScrollingLayout::AlignChildImpl( UIElement* elem )
{
  switch (m_align)
  {
  case  AL_LOW:
    elem->x = x;
    break;

  case  AL_CENTER:
    elem->x = x + (w - elem->w) / 2;
    break;

  case  AL_HIGH:
    elem->x = x + w - elem->w;
    break;

  case  AL_NOALIGN:
    break;
  }

  if (m_children.size() > 0)
  {
    if (m_growDir == GD_POSITIVE)
    {
      elem->y = m_children.back()->y + m_children.back()->h + m_spacing;
    }
    else
    {
      elem->y = m_children.back()->y - (elem->h + m_spacing);
    }
  }
  else
  {
    if (m_growDir == GD_POSITIVE)
    {
      elem->y = y;
    }
    else
    {
      elem->y = y + h - elem->h;
    }

    elem->y += m_yOffset;
  }

  elem->OnChange();
}

//==============================================================================
void UIVerticalScrollingLayout::Render(IUIRenderer& renderer) const
{
  renderer.Render();

  {
    ScissorStack::Scope s3(this, true);

    UIVerticalLayout::Render(renderer);
    renderer.Render();
  }
}

//==============================================================================
void UIVerticalScrollingLayout::SetYOffset(int yOffs)
{
  if (yOffs > 0)
  {
    yOffs = 0;
  }
  else
  {
    int eh(CalculateElementsExtent());
    if (eh < h)
    {
      eh = h;
    }

    int minOffs(h - eh);
    if (yOffs < minOffs)
    {
      yOffs = minOffs;
    }
  }

  if (m_yOffset != yOffs)
  {
    m_yOffset = yOffs;
    OnChange();
  }
}

//==============================================================================
void UIVerticalScrollingLayout::SetSensitivity(float s)
{
  m_sensitivity = s;
}

} // xr

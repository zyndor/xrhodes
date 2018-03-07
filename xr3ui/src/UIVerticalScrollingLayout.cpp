//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UIVerticalScrollingLayout.hpp"
#include "XR/UIRenderer.hpp"
#include "XR/Gfx.hpp"

namespace XR
{

//==============================================================================
UIVerticalScrollingLayout::UIVerticalScrollingLayout()
: UIVerticalLayout(),
  m_yOffset(0),
  m_sensitivity(1.0f),
  m_isTouched(false),
  m_lastTouchY(0)
{}

//==============================================================================
UIVerticalScrollingLayout::~UIVerticalScrollingLayout()
{}

//==============================================================================
bool UIVerticalScrollingLayout::OnMouseAction(const Input::MouseActionEvent& e )
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
bool UIVerticalScrollingLayout::OnMouseMotion(const Input::MouseMotionEvent& e )
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
void UIVerticalScrollingLayout::_AlignElement( UIElement* pElem )
{
  switch (m_align)
  {
  case  AL_LOW:
    pElem->x = x;
    break;

  case  AL_CENTER:
    pElem->x = x + (w - pElem->w) / 2;
    break;

  case  AL_HIGH:
    pElem->x = x + w - pElem->w;
    break;

  case  AL_NOALIGN:
    break;
  }

  if (m_lElements.size() > 0)
  {
    if (m_growDir == GD_POSITIVE)
    {
      pElem->y = m_lElements.back()->y + m_lElements.back()->h + m_spacing;
    }
    else
    {
      pElem->y = m_lElements.back()->y - (pElem->h + m_spacing);
    }
  }
  else
  {
    if (m_growDir == GD_POSITIVE)
    {
      pElem->y = y;
    }
    else
    {
      pElem->y = y + h - pElem->h;
    }

    pElem->y += m_yOffset;
  }

  pElem->OnChange();
}

//==============================================================================
void UIVerticalScrollingLayout::Render(IUIRenderer& renderer) const
{
  renderer.Render();

  Gfx::SetScissor(this);

  UIVerticalLayout::Render(renderer);
  renderer.Render();

  Gfx::SetScissor(nullptr);
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

} // XR

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/UICheckBox.hpp"
#include "xr/UIRenderer.hpp"

namespace xr
{

//==============================================================================
UICheckBox::UICheckBox()
: UIButton(),
  onSelectedStateChanged(nullptr),
  setSprite()
{}

//==============================================================================
void UICheckBox::SetSelected( bool isIt )
{
  m_state = (m_state & ~MASK_SELECTED) | (MASK_SELECTED * isIt);
}

//==============================================================================
bool  UICheckBox::OnMouseAction(const Input::MouseActionData& e )
{
  const int kRight(CalculateRight());
  const int kBottom(CalculateBottom());

  bool  isHandled(false);
  if ((!m_activeArea ||
    (e.x >= m_activeArea->x &&
    e.y >= m_activeArea->y &&
    e.x < m_activeArea->x + m_activeArea->w &&
    e.y < m_activeArea->y + m_activeArea->h)) &&
    (e.x >= x && e.x < kRight &&
    e.y >= y && e.y < kBottom))
  {
    if (e.isPressed)
    {
      if (!IsPressed())
      {
        m_state |= MASK_PRESSED;  // press
        m_lastTouch.x = e.x;
        m_lastTouch.y = e.y;
        if (IsEnabled())
        {
          OnPressed();
        }
        isHandled = true;
      }
    }
    else if (IsPressed())
    {
      m_state &= ~MASK_PRESSED;  // release

      if (IsEnabled() &&
        m_lastTouch.x >= x && m_lastTouch.x < kRight &&
        m_lastTouch.y >= y && m_lastTouch.y < kBottom)
      {
        OnReleased();
        SetSelected(!IsSelected());
        OnSelectedStateChanged();
      }
      isHandled = true;
    }
  }
  else if (IsPressed())
  {
    m_state &= ~MASK_PRESSED; // unpress
  }
  return isHandled;
}

//==============================================================================
void UICheckBox::Render(IUIRenderer& renderer) const
{
  UIButton::Render(renderer);

  if (IsSelected())
  {
    XR_ASSERTMSG(IW_ASSERTION_CHANNEL_DEFAULT, setMaterial != nullptr,
      ("Material needs to be set in UICheckBox::setSprite before Render()"));

    auto verts = renderer.NewSprite(setMaterial);
    setSprite.CopyUVsTo(verts);
    CalculateSpriteVertices(&setSprite, verts);
    ApplyColor(verts);
  }
}

//==============================================================================
void  UICheckBox::OnSelectedStateChanged()
{
  if (onSelectedStateChanged)
  {
    (*onSelectedStateChanged)(this, callbackData);
  }
}

} // xr

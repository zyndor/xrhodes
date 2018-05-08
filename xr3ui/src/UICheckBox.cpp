//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UICheckBox.hpp"
#include "XR/UIRenderer.hpp"

namespace XR
{

//==============================================================================
UICheckBox::UICheckBox()
: UIButton(),
  pOnSelectedStateChanged(0),
  setSprite()
{}

//==============================================================================
UICheckBox::~UICheckBox()
{}

//==============================================================================
void UICheckBox::SetSelected( bool isIt )
{
  m_state = (m_state & ~MASK_SELECTED) | (MASK_SELECTED * isIt);
}

//==============================================================================
bool  UICheckBox::OnMouseAction(const Input::MouseActionEvent& e )
{
  const int kRight(CalculateRight());
  const int kBottom(CalculateBottom());

  bool  isHandled(false);
  if ((m_pActiveArea == 0 ||
    (e.x >= m_pActiveArea->x &&
    e.y >= m_pActiveArea->y &&
    e.x < m_pActiveArea->x + m_pActiveArea->w &&
    e.y < m_pActiveArea->y + m_pActiveArea->h)) &&
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
    _CalculateSpriteVerts(&setSprite, verts);
    _ApplyColor(verts);
  }
}

//==============================================================================
void  UICheckBox::OnSelectedStateChanged()
{
  if (pOnSelectedStateChanged != 0)
  {
    (*pOnSelectedStateChanged)(this, pCallbackData);
  }
}

} // XR

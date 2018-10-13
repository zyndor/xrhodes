//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/UIButton.hpp"
#include "xr/UIRenderer.hpp"

namespace xr
{

//==============================================================================
UIButton::UIButton()
: UIColoredElement(),
  sprites(),
  onPressed(nullptr),
  onReleased(nullptr),
  callbackData(nullptr),
  m_activeArea(nullptr),
  m_state(MASK_ENABLED),
  m_lastTouch(-1, -1)
{}

//==============================================================================
void UIButton::SetEnabled( bool isIt )
{
  m_state = (m_state & ~MASK_ENABLED) | (MASK_ENABLED * isIt);
}

//==============================================================================
void UIButton::SetPressed( bool isIt )
{
  m_state = (m_state & ~MASK_PRESSED) | (MASK_PRESSED * isIt);
}

//==============================================================================
bool  UIButton::OnMouseAction(const Input::MouseActionData& e )
{
  const int kRight = CalculateRight();
  const int kBottom = CalculateBottom();

  bool  isHandled(false);
  if ((m_activeArea == nullptr ||
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
bool  UIButton::OnMouseMotion(const Input::MouseMotionData& e )
{
  if (IsEnabled() && IsPressed() &&
    (e.x < x || e.x >= (x + w) ||
      e.y < y || e.y >= (y + h)))
  {
    m_state &= ~MASK_PRESSED;
  }
  return false;
}

//==============================================================================
void UIButton::Render(IUIRenderer& renderer) const
{
  auto const spriteId = GetSpriteId();
  const Sprite* sprite = sprites + spriteId;
  auto const& material = materials[spriteId];
  XR_ASSERTMSG(UIButton, material != nullptr,
    ("Material[%d] needs to be set before Render().", GetSpriteId()));

  auto verts = renderer.NewSprite(material);
  sprite->CopyUVsTo(verts);
  CalculateSpriteVertices(sprite, verts);
  ApplyColor(verts);
}

//==============================================================================
void  UIButton::SetSprites(const Sprite* sprite, float scale)
{
  XR_ASSERT(UIButton, sprite != nullptr);
  sprites[S_UP] = sprites[S_DOWN] = sprites[S_OFF] = *sprite;

  SetSizeToSprite(scale);
}

//==============================================================================
void  UIButton::SetSprites(const Sprite* sprites_[kNumStates], float scale)
{
  XR_ASSERT(UIButton, sprites_ != nullptr);
  for (int i = 0; i < kNumStates; ++i)
  {
    sprites[i] = *sprites_[i];
  }

  SetSizeToSprite(scale);
}

//==============================================================================
void UIButton::SetSizeToSprite(float scale)
{
  scale *= 2.0f;
  w = static_cast<int32_t>(Round(sprites[0].GetHalfWidth() * scale));
  h = static_cast<int32_t>(Round(sprites[0].GetHalfHeight() * scale));
}

//==============================================================================
int UIButton::GetSpriteId() const
{
  return IsEnabled() ? (IsPressed() ? S_DOWN : S_UP) : S_OFF;
}

//==============================================================================
void UIButton::SetActiveArea( const Rect* pActiveArea )
{
  m_activeArea = pActiveArea;
}

//==============================================================================
void UIButton::OnPressed()
{
  if (onPressed)
  {
    (*onPressed)(this, callbackData);
  }
}

//==============================================================================
void UIButton::OnReleased()
{
  if (onReleased)
  {
    (*onReleased)(this, callbackData);
  }
}

} // xr

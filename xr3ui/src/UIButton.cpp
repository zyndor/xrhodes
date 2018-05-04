//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UIButton.hpp"
#include "XR/UIRenderer.hpp"

namespace XR
{

//==============================================================================
UIButton::UIButton()
: UIColoredElement(),
  sprites(),
  pOnPressed(0),
  pOnReleased(0),
  pCallbackData(0),
  m_pActiveArea(0),
  m_state(MASK_ENABLED),
  m_lastTouch(-1, -1)
{}

//==============================================================================
UIButton::~UIButton()
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
bool  UIButton::OnMouseAction(const Input::MouseActionEvent& e )
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
bool  UIButton::OnMouseMotion(const Input::MouseMotionEvent& e )
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
  const Sprite* sprite(sprites + spriteId);
  auto const& material = materials[spriteId];
  XR_ASSERTMSG(UIButton, material != nullptr,
    ("Material needs to be set in UIButton::arSprites[%d] before Render()",
    GetSpriteId()));

  auto verts = renderer.NewSprite(material);
  sprite->CopyUVsTo(verts);
  _CalculateSpriteVerts(sprite, verts);
  _ApplyColor(verts);
}

//==============================================================================
void  UIButton::SetSprites(const Sprite* pSprite, float scale)
{
  XR_ASSERT(UIButton, pSprite != 0);
  sprites[S_UP] = sprites[S_DOWN] = sprites[S_OFF] = *pSprite;

  SetSizeToSprite(scale);
}

//==============================================================================
void  UIButton::SetSprites(const Sprite* arpSprite[kNumStates], float scale)
{
  XR_ASSERT(UIButton, arpSprite != 0);
  for (int i = 0; i < kNumStates; ++i)
  {
    sprites[i] = *arpSprite[i];
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

void UIButton::SetActiveArea( const Rect* pActiveArea )
{
  m_pActiveArea = pActiveArea;
}

//==============================================================================
void UIButton::OnPressed()
{
  if (pOnPressed != 0)
  {
    (*pOnPressed)(this, pCallbackData);
  }
}

//==============================================================================
void UIButton::OnReleased()
{
  if (pOnReleased != 0)
  {
    (*pOnReleased)(this, pCallbackData);
  }
}

} // XR

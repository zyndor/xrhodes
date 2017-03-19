//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIButton.hpp"
#include "UIRenderer.hpp"

namespace XR
{

//==============================================================================
UIButton::UIButton()
: UIColoredElement(),
  arSprite(),
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
void UIButton::Render() const
{
  const Sprite* pSprite(arSprite + GetSpriteId());
  XR_ASSERTMSG(UIButton, pSprite->GetMaterial() != 0,
    ("Material needs to be set in UIButton::arSprites[%d] before Render()",
    GetSpriteId()));

  FloatBuffer* pFbVerts = Renderer::AllocBuffer(sizeof(Vector3),
    Sprite::kNumVertices);
  _CalculateSpriteVerts(pSprite, *pFbVerts);

  FloatBuffer* pFbUVs = pSprite->CopyUVs();

  Renderer::SetMaterial(pSprite->GetMaterial());
  Renderer::SetAmbientColor(color);

  Renderer::SetVertStream(*pFbVerts);
  Renderer::SetUVStream(*pFbUVs);

  Renderer::DrawPrims(PRIM_TRI_LIST, Sprite::karIndices, Sprite::kNumIndices);
}

//==============================================================================
void UIButton::Render( UIRenderer* pRenderer ) const
{
  const Sprite* pSprite(arSprite + GetSpriteId());
  XR_ASSERTMSG(UIButton, pSprite->GetMaterial() != 0,
    ("Material needs to be set in UIButton::arSprites[%d] before Render()",
    GetSpriteId()));

  FloatBuffer  fbVerts = pRenderer->NewSprite(pSprite->GetMaterial(),
    pSprite->GetUVs(), color);
  _CalculateSpriteVerts(pSprite, fbVerts);
}

//==============================================================================
void  UIButton::SetSprites(const Sprite* pSprite, float scale)
{
  XR_ASSERT(UIButton, pSprite != 0);
  arSprite[S_UP] = arSprite[S_DOWN] = arSprite[S_OFF] = *pSprite;
  
  SetSizeToSprite(scale);
}
  
//==============================================================================
void  UIButton::SetSprites(const Sprite* arpSprite[kNumStates], float scale)
{
  XR_ASSERT(UIButton, arpSprite != 0);
  for (int i = 0; i < kNumStates; ++i)
  {
    arSprite[i] = *arpSprite[i];
  }

  SetSizeToSprite(scale);
}

//==============================================================================
void UIButton::SetSizeToSprite(float scale)
{
  scale *= 2.0f;
  w = Round(arSprite[0].GetHalfWidth() * scale);
  h = Round(arSprite[0].GetHalfHeight() * scale);
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

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UIColoredElement.hpp"
#include "XR/Renderer.hpp"

namespace XR
{

//==============================================================================
UIColoredElement::UIColoredElement()
: UIElement(),
  color(0xffffffff)
{}

//==============================================================================
UIColoredElement::~UIColoredElement()
{}

//==============================================================================
void  UIColoredElement::_CalculateSpriteVerts( const Sprite* pSprite,
        FloatBuffer& fbVerts) const
{
  XR_ASSERT(UIColoredElement, pSprite != 0);
  XR_ASSERT(UIColoredElement, pSprite->GetMaterial() != 0);
  XR_ASSERT(UIColoredElement, fbVerts.GetElementSizeBytes() == sizeof(Vector3));
  XR_ASSERT(UIColoredElement, fbVerts.GetNumElements() == Sprite::kNumVertices);

  float ws(w / (pSprite->GetHalfWidth() * 2.0f));
  float hs(h / (pSprite->GetHalfHeight() * 2.0f));
  float left(x + static_cast<int32_t>(Round(pSprite->GetLeftPadding() * ws)));
  float right(x + w - static_cast<int32_t>(Round(pSprite->GetRightPadding() * ws)));
  float top(y + static_cast<int32_t>(Round(pSprite->GetTopPadding() * hs)));
  float bottom(y + h - static_cast<int32_t>(Round(pSprite->GetBottomPadding() * hs)));

  fbVerts.Set(Sprite::VI_NW, Vector3(left, top, .0f)); 
  fbVerts.Set(Sprite::VI_SW, Vector3(left, bottom, .0f)); 
  fbVerts.Set(Sprite::VI_SE, Vector3(right, bottom, .0f)); 
  fbVerts.Set(Sprite::VI_NE, Vector3(right, top, .0f));
}

} // XR

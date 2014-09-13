#include <XR/Renderer.hpp>
#include "UIColoredElement.hpp"

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
        RenderStream& rsVerts) const
{
  XR_ASSERT(UIColoredElement, pSprite != 0);
  XR_ASSERT(UIColoredElement, pSprite->GetMaterial() != 0);
  XR_ASSERT(UIColoredElement, rsVerts.GetFormat() == RenderStream::F_VECTOR3);
  XR_ASSERT(UIColoredElement, rsVerts.GetCapacity() == Sprite::kNumVertices);

  float ws(w / (pSprite->GetHalfWidth() * 2.0f));
  float hs(h / (pSprite->GetHalfHeight() * 2.0f));
  float left(x + static_cast<int16>(Round(pSprite->GetLeftPadding() * ws)));
  float right(x + w - static_cast<int16>(Round(pSprite->GetRightPadding() * ws)));
  float top(y + static_cast<int16>(Round(pSprite->GetTopPadding() * hs)));
  float bottom(y + h - static_cast<int16>(Round(pSprite->GetBottomPadding() * hs)));

  rsVerts.Set(Sprite::VI_NW, Vector3(left, top, .0f)); 
  rsVerts.Set(Sprite::VI_SW, Vector3(left, bottom, .0f)); 
  rsVerts.Set(Sprite::VI_SE, Vector3(right, bottom, .0f)); 
  rsVerts.Set(Sprite::VI_NE, Vector3(right, top, .0f));
}

} // XR

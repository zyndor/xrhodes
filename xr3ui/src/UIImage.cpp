//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UIImage.hpp"
#include "XR/UIRenderer.hpp"

namespace XR
{

//==============================================================================
UIImage::UIImage()
: UIColoredElement(),
  sprite()
{}

//==============================================================================
UIImage::~UIImage()
{}

//==============================================================================
void  UIImage::SetSprite(const Sprite *pSprite, float scale)
{
  XR_ASSERT(UIImage, pSprite != 0);
  sprite = *pSprite;

  SetSizeToSprite(scale);
}

//==============================================================================
void UIImage::SetSizeToSprite(float scale)
{
  scale *= 2.0f;
  w = static_cast<int32_t>(Round(sprite.GetHalfWidth() * scale));
  h = static_cast<int32_t>(Round(sprite.GetHalfHeight() * scale));
}

//==============================================================================
void UIImage::Render(IUIRenderer& renderer) const
{
  XR_ASSERTMSG(UIImage, material != nullptr,
    ("Material needs to be set in UIImage::sprite before Render()"));

  auto verts = renderer.NewSprite(material);
  sprite.CopyUVsTo(verts);
  _CalculateSpriteVerts(&sprite, verts);
  _ApplyColor(verts);
}

} // XR

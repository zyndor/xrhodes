//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIImage.hpp"
#include "UIRenderer.hpp"

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
  w = Round(sprite.GetHalfWidth() * scale);
  h = Round(sprite.GetHalfHeight() * scale);
}
  
//==============================================================================
void UIImage::Render() const
{
  XR_ASSERTMSG(UIImage, sprite.GetMaterial() != 0,
    ("Material needs to be set in UIImage::sprite before Render()"));

  FloatBuffer* pFbVerts = Renderer::AllocBuffer(sizeof(Vector3),
    Sprite::kNumVertices);
  _CalculateSpriteVerts(&sprite, *pFbVerts);

  FloatBuffer* pFbUVs = sprite.CopyUVs();

  Renderer::SetMaterial(sprite.GetMaterial());
  Renderer::SetAmbientColor(color);

  Renderer::SetVertStream(*pFbVerts);
  Renderer::SetUVStream(*pFbUVs);

  Renderer::DrawPrims(PRIM_TRI_LIST, Sprite::karIndices, Sprite::kNumIndices);
}

//==============================================================================
void UIImage::Render( UIRenderer* pRenderer ) const
{
  XR_ASSERTMSG(UIImage, sprite.GetMaterial() != 0,
    ("Material needs to be set in UIImage::sprite before Render()"));

  FloatBuffer fbVerts = pRenderer->NewSprite(sprite.GetMaterial(),
    sprite.GetUVs(), color);
  _CalculateSpriteVerts(&sprite, fbVerts);
}

} // XR

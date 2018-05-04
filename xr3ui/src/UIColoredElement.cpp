//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UIColoredElement.hpp"

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
void  UIColoredElement::_CalculateSpriteVerts(const Sprite* sprite,
  IUIRenderer::Vertex verts[Sprite::kNumVertices]) const
{
  XR_ASSERT(UIColoredElement, sprite != 0);

  float ws = (w / 2) / sprite->GetHalfWidth();
  float hs = (h / 2) / sprite->GetHalfHeight();
  float left = x + Round(sprite->GetLeftPadding() * ws);
  float right = x + w - Round(sprite->GetRightPadding() * ws);
  float top = y + Round(sprite->GetTopPadding() * hs);
  float bottom = y + h - Round(sprite->GetBottomPadding() * hs);

  verts[Sprite::VI_NW].pos = Vector3(left, top, .0f);
  verts[Sprite::VI_SW].pos = Vector3(left, bottom, .0f);
  verts[Sprite::VI_SE].pos = Vector3(right, bottom, .0f);
  verts[Sprite::VI_NE].pos = Vector3(right, top, .0f);
}

//==============================================================================
void UIColoredElement::_ApplyColor(IUIRenderer::Vertex verts[Sprite::kNumVertices]) const
{
  for(int i = 0; i < Sprite::kNumVertices; ++i)
  {
    verts[i].color0 = color;
  }
}

} // XR

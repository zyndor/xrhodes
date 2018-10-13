//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/UIColoredElement.hpp"

namespace xr
{

//==============================================================================
UIColoredElement::UIColoredElement()
: UIElement(),
  color(0xffffffff)
{}

//==============================================================================
void  UIColoredElement::CalculateSpriteVertices(const Sprite* sprite,
  IUIRenderer::Vertex verts[Quad::Vertex::kCount]) const
{
  XR_ASSERT(UIColoredElement, sprite != nullptr);

  float ws = (w / 2) / sprite->GetHalfWidth();
  float hs = (h / 2) / sprite->GetHalfHeight();
  float left = x + Round(sprite->GetLeftPadding() * ws);
  float right = x + w - Round(sprite->GetRightPadding() * ws);
  float top = y + Round(sprite->GetTopPadding() * hs);
  float bottom = y + h - Round(sprite->GetBottomPadding() * hs);

  verts[Quad::Vertex::NW].pos = Vector3(left, top, .0f);
  verts[Quad::Vertex::NE].pos = Vector3(right, top, .0f);
  verts[Quad::Vertex::SW].pos = Vector3(left, bottom, .0f);
  verts[Quad::Vertex::SE].pos = Vector3(right, bottom, .0f);
}

//==============================================================================
void UIColoredElement::ApplyColor(IUIRenderer::Vertex verts[Quad::Vertex::kCount]) const
{
  for(int i = 0; i < Quad::Vertex::kCount; ++i)
  {
    verts[i].color0 = color;
  }
}

} // xr

#ifndef XR_UICOLOREDELEMENT_HPP
#define XR_UICOLOREDELEMENT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/maths.hpp"
#include "xr/Sprite.hpp"
#include "UIElement.hpp"
#include "IUIRenderer.hpp"

namespace xr
{

//==============================================================================
///@brief Base for UIElements that have a color.
class UIColoredElement: public UIElement
{
public:
  // data
  Color color;

  // structors
  UIColoredElement();

protected:
  // internal
  void CalculateSpriteVertices(const Sprite* sprite,
    IUIRenderer::Vertex verts[Quad::Vertex::kCount]) const;
  void ApplyColor(IUIRenderer::Vertex verts[Quad::Vertex::kCount]) const;
};

} // xr

#endif // XR_UICOLOREDELEMENT_HPP

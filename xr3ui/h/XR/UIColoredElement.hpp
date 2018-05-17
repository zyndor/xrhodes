#ifndef XR_UICOLOREDELEMENT_HPP
#define XR_UICOLOREDELEMENT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/maths.hpp"
#include "XR/Sprite.hpp"
#include "UIElement.hpp"
#include "IUIRenderer.hpp"

namespace XR
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
  virtual ~UIColoredElement();

protected:
  // internal
  void _CalculateSpriteVerts(const Sprite* sprite,
    IUIRenderer::Vertex verts[Quad::Vertex::kCount]) const;
  void _ApplyColor(IUIRenderer::Vertex verts[Quad::Vertex::kCount]) const;
};

} // XR

#endif // XR_UICOLOREDELEMENT_HPP

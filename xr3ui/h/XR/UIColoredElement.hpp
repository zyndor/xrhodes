//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UICOLOREDELEMENT_HPP
#define XR_UICOLOREDELEMENT_HPP

#include <XR/maths.hpp>
#include <XR/Sprite.hpp>
#include "UIElement.hpp"

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
  void _CalculateSpriteVerts(const Sprite* pSprite, RenderStream& rsVerts) const;
};

} // XR

#endif // XR_UICOLOREDELEMENT_HPP
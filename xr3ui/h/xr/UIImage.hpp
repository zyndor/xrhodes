#ifndef XR_UIIMAGE_HPP
#define XR_UIIMAGE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/Sprite.hpp"
#include "UIColoredElement.hpp"

namespace xr
{

//==============================================================================
///@brief UIElement representing a single, static image.
class UIImage:  public UIColoredElement
{
public:
  // data
  Sprite  sprite;
  Material::Ptr material; // TODO: UIBuilder support

  // structors
  UIImage();

  // general use
  void  SetSprite(const Sprite* sprite, float scale = 1.0f);
  void  SetSizeToSprite(float scale = 1.0f);

  void Render(IUIRenderer& renderer) const override;
};

} // xr

#endif // XR_UIIMAGE_HPP

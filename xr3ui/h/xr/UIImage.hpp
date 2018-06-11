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
class UIImage:  public UIColoredElement
{
public:
  // data
  Sprite  sprite;
  Material::Ptr material; // TODO: UIBuilder support

  // structors
  UIImage();
  virtual ~UIImage();

  // general use
  void  SetSprite(const Sprite* sprite, float scale = 1.0f);
  void  SetSizeToSprite(float scale = 1.0f);

  virtual void Render(IUIRenderer& renderer) const;
};

} // xr

#endif // XR_UIIMAGE_HPP
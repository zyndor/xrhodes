#ifndef XR_UIIMAGE_HPP
#define XR_UIIMAGE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Sprite.hpp"
#include "UIColoredElement.hpp"

namespace XR
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
  void  SetSprite(const Sprite* pSprite, float scale = 1.0f);
  void  SetSizeToSprite(float scale = 1.0f);

  virtual void Render(IUIRenderer& renderer) const;
};

} // XR

#endif // XR_UIIMAGE_HPP

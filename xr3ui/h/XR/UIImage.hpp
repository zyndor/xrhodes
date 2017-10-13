//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UIIMAGE_HPP
#define XR_UIIMAGE_HPP

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

  // structors
  UIImage();  
  virtual ~UIImage();

  // general use
  void  SetSprite(const Sprite* pSprite, float scale = 1.0f);
  void  SetSizeToSprite(float scale = 1.0f);

  virtual void Render() const;
  virtual void Render(UIRenderer* pRenderer) const;
};

} // XR

#endif // XR_UIIMAGE_HPP

//
// Nuclear Heart Games
// XRhodes
//
// UIImage.hpp
// @author  Gyorgy Straub
// @date    21/06/2012
//
// copyright (c) 2011 - 2015. All rights reserved.
//
//==============================================================================
#if !defined XR_UIIMAGE_HPP
#define XR_UIIMAGE_HPP

#include <XR/Sprite.hpp>
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

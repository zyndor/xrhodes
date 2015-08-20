//
// Nuclear Heart Games
// XRhodes
//
// UIImagePanel.hpp
// @author  Gyorgy Straub
// @date    22/05/2013
//
// copyright (c) 2011 - 2015. All rights reserved.
//
//==============================================================================
#if !defined XR_UIIMAGEPANEL_HPP
#define XR_UIIMAGEPANEL_HPP

#include "UIImage.hpp"

namespace XR
{

//==============================================================================
class UIImagePanel:  public UIImage
{
public:
  // data
  float hSplit;
  float vSplit;

  // structors
  UIImagePanel();  
  virtual ~UIImagePanel();

  // general use
  virtual void Render() const;
  virtual void Render(UIRenderer* pRenderer) const;
};

} // XR

#endif // XR_UIIMAGEPANEL_HPP

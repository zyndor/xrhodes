//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UIIMAGEPANEL_HPP
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

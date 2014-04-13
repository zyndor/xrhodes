//
// Nuclear Heart Games
// XRhodes
//
// UISpacer.hpp
// @author  Gyorgy Straub
// @date    21/06/2012
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UISPACER_HPP
#define XR_UISPACER_HPP

#include "UIElement.hpp"

namespace XR
{

//==============================================================================
class UISpacer: public UIElement
{
public:
  // structors
  UISpacer();
  ~UISpacer();

  // general
  virtual void Render() const;
  virtual void Render(UIRenderer* pRenderer) const;
};

} // XR

#endif // XR_UISPACER_HPP

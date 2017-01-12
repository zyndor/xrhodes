//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UISPACER_HPP
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

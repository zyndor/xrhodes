#ifndef XR_UISPACER_HPP
#define XR_UISPACER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
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
  void Render(IUIRenderer& renderer) const override;
};

} // XR

#endif // XR_UISPACER_HPP

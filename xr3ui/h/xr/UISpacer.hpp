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

namespace xr
{

//==============================================================================
///@brief An UIElement whose sole purpose is taking up space.
class UISpacer: public UIElement
{
public:
  // structors
  UISpacer();

  // general
  void Render(IUIRenderer& renderer) const override;
};

} // xr

#endif // XR_UISPACER_HPP

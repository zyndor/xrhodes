#ifndef XR_UIVERTICALSLIDER_HPP
#define XR_UIVERTICALSLIDER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "UISliderBase.hpp"

namespace xr
{

//==============================================================================
///@brief A slider where the movement of the handle is vertical.
class UIVerticalSlider: public UISliderBase
{
public:
  // structors
  UIVerticalSlider();

  // general use
  int32_t CalculateRange() const override;

  bool  OnMouseAction(const Input::MouseActionEvent& e) override;
  bool  OnMouseMotion(const Input::MouseMotionEvent& e) override;

  void  Render(IUIRenderer& renderer) const override;
};

} // xr

#endif // XR_UIVERTICALSLIDER_HPP

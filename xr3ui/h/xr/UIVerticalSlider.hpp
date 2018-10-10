#ifndef XR_UIVERTICALSLIDER_HPP
#define XR_UIVERTICALSLIDER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
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

  bool  OnMouseAction(const Input::MouseActionData& e) override;
  bool  OnMouseMotion(const Input::MouseMotionData& e) override;

  void  Render(IUIRenderer& renderer) const override;
};

} // xr

#endif // XR_UIVERTICALSLIDER_HPP

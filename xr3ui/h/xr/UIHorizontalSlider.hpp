#ifndef XR_UIHORIZONTALSLIDER_HPP
#define XR_UIHORIZONTALSLIDER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UISliderBase.hpp"

namespace xr
{

//==============================================================================
///@brief A slider where the movement of the handle is horizontal.
  class UIHorizontalSlider: public UISliderBase
{
public:
  // structors
  UIHorizontalSlider();

  // general use
  int32_t CalculateRange() const override;

  bool  OnMouseAction(const Input::MouseActionEvent& e) override;
  bool  OnMouseMotion(const Input::MouseMotionEvent& e) override;

  void  Render(IUIRenderer& renderer) const override;
};

} // xr

#endif // XR_UIHORIZONTALSLIDER_HPP

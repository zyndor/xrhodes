#ifndef XR_UIVERTICALSLIDER_HPP
#define XR_UIVERTICALSLIDER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UISliderBase.hpp"

namespace XR
{

//==============================================================================
class UIVerticalSlider: public UISliderBase
{
public:
  // structors
  UIVerticalSlider();
  ~UIVerticalSlider();

  // general use
  int32_t CalculateRange() const override;

  bool  OnMouseAction(const Input::MouseActionEvent& e) override;
  bool  OnMouseMotion(const Input::MouseMotionEvent& e) override;

  void  Render(IUIRenderer& renderer) const override;
};

} // XR

#endif // XR_UIVERTICALSLIDER_HPP

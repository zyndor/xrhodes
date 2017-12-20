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

namespace XR
{

//==============================================================================
class UIHorizontalSlider: public UISliderBase
{
public:
  // structors
  UIHorizontalSlider();
  ~UIHorizontalSlider();

  // general use
  virtual int CalculateRange() const;

  virtual bool  OnMouseAction(const Input::MouseActionEvent& e);
  virtual bool  OnMouseMotion(const Input::MouseMotionEvent& e);

  void  Render(IUIRenderer& renderer) const override;
};

} // XR

#endif // XR_UIHORIZONTALSLIDER_HPP

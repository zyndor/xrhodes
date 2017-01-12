//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UIVERTICALSLIDER_HPP
#define XR_UIVERTICALSLIDER_HPP

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
  virtual int   CalculateRange() const;

  virtual bool  OnMouseAction(const Input::MouseActionEvent& e);
  virtual bool  OnMouseMotion(const Input::MouseMotionEvent& e);

  virtual void  Render() const;
  virtual void  Render(UIRenderer* pRenderer) const;
};

} // XR

#endif // XR_UIVERTICALSLIDER_HPP

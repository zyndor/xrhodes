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

  virtual void  Render() const;
  virtual void  Render(UIRenderer* pRenderer) const;
};

} // XR

#endif // XR_UIHORIZONTALSLIDER_HPP
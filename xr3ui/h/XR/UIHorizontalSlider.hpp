//
// Nuclear Heart Games
// XRhodes
//
// UIHorizontalSlider.hpp
// @author  Gyorgy Straub
// @date    17/01/2013
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UIHORIZONTALSLIDER_HPP
#define XR_UIHORIZONTALSLIDER_HPP

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
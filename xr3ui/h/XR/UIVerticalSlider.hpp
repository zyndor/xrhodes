//
// Nuclear Heart Games
// XRhodes
//
// UIVerticalSlider.hpp
// @author  Gyorgy Straub
// @date    16/07/2012
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UIVERTICALSLIDER_HPP
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

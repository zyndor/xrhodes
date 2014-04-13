//
// Nuclear Heart Games
// XRhodes
//
// UIHorizontalProgressBar.hpp
// @author  Gyorgy Straub
// @date    07/01/2013
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UIHORIZONTALPROGRESSBAR_HPP
#define XR_UIHORIZONTALPROGRESSBAR_HPP

#include "UIProgressBarBase.hpp"

namespace XR
{

//==============================================================================
class UIHorizontalProgressBar:  public UIProgressBarBase
{
public:
  // structors
  UIHorizontalProgressBar();
  ~UIHorizontalProgressBar();

  // general
  virtual void  Render() const;
  virtual void  Render(UIRenderer* pRenderer) const;
};

} // XR

#endif // XR_UIHORIZONTALPROGRESSBAR_HPP
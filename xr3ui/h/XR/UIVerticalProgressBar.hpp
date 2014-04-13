//
// Nuclear Heart Games
// XRhodes
//
// UIVerticalProgressBar.hpp
// @author  Gyorgy Straub
// @date    07/01/2013
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UIVERTICALPROGRESSBAR_HPP
#define XR_UIVERTICALPROGRESSBAR_HPP

#include "UIProgressBarBase.hpp"

namespace XR
{

//==============================================================================
class UIVerticalProgressBar:  public UIProgressBarBase
{
public:
  // structors
  UIVerticalProgressBar();
  ~UIVerticalProgressBar();

  // general
  virtual void  Render() const;
  virtual void  Render(UIRenderer* pRenderer) const;
};

} // XR

#endif // XR_UIVERTICALPROGRESSBAR_HPP
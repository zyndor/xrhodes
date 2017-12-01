#ifndef XR_UIVERTICALPROGRESSBAR_HPP
#define XR_UIVERTICALPROGRESSBAR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

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
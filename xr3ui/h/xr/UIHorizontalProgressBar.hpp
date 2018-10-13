#ifndef XR_UIHORIZONTALPROGRESSBAR_HPP
#define XR_UIHORIZONTALPROGRESSBAR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "UIProgressBarBase.hpp"

namespace xr
{

//==============================================================================
class UIHorizontalProgressBar:  public UIProgressBarBase
{
public:
  // structors
  UIHorizontalProgressBar();

  // general
  void  Render(IUIRenderer& renderer) const override;
};

} // xr

#endif // XR_UIHORIZONTALPROGRESSBAR_HPP

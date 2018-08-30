#ifndef XR_UIVERTICALPROGRESSBAR_HPP
#define XR_UIVERTICALPROGRESSBAR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "UIProgressBarBase.hpp"

namespace xr
{

//==============================================================================
class UIVerticalProgressBar:  public UIProgressBarBase
{
public:
  // structors
  UIVerticalProgressBar();

  // general
  void  Render(IUIRenderer& renderer) const override;
};

} // xr

#endif // XR_UIVERTICALPROGRESSBAR_HPP

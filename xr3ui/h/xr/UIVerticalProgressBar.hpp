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

namespace xr
{

//==============================================================================
class UIVerticalProgressBar:  public UIProgressBarBase
{
public:
  // structors
  UIVerticalProgressBar();
  ~UIVerticalProgressBar();

  // general
  void  Render(IUIRenderer& renderer) const override;
};

} // xr

#endif // XR_UIVERTICALPROGRESSBAR_HPP

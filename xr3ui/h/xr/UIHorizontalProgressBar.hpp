#ifndef XR_UIHORIZONTALPROGRESSBAR_HPP
#define XR_UIHORIZONTALPROGRESSBAR_HPP
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

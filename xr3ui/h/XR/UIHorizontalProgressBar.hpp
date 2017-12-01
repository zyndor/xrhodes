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
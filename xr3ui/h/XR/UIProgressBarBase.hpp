#ifndef XR_UIPROGRESSBARBASE_HPP
#define XR_UIPROGRESSBARBASE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "UIImage.hpp"

namespace XR
{

//==============================================================================
class UIProgressBarBase:  public UIImage
{
public:
  // enum
  enum  FillDir // fill direction
  {
    FD_POSITIVE,
    FD_NEGATIVE
  };

  // structors
  explicit UIProgressBarBase(FillDir fd);
  ~UIProgressBarBase();

  // general
  FillDir GetFillDirection() const;
  float   GetPercentage() const;

  void    SetFillDirection(FillDir fd);
  void    SetPercentage(float perc);

protected:
  // data
  FillDir m_fillDir;
  float   m_percentage;
};

} // XR

#endif // XR_UIPROGRESSBARBASE_HPP
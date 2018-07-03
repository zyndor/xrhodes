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

namespace xr
{

//==============================================================================
///@brief Base class for UIElements representing a normalized numeric value in
/// the range of [0.0; 1.0].
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

} // xr

#endif // XR_UIPROGRESSBARBASE_HPP

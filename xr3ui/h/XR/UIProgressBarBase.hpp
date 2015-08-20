//
// Nuclear Heart Games
// XRhodes
//
// UIProgressBarBase.hpp
// @author  Gyorgy Straub
// @date    07/01/2013
//
// copyright (c) 2011 - 2015. All rights reserved.
//
//==============================================================================
#if !defined XR_UIPROGRESSBARBASE_HPP
#define XR_UIPROGRESSBARBASE_HPP

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
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/UISliderBase.hpp"
#include <limits>

namespace xr
{

//==============================================================================
UISliderBase::UISliderBase()
: UIImage(),
  onValueChangeCb(nullptr),
  onValueChangeCbData(nullptr),
  m_percentage(.0f),
  m_isTouched(false),
  m_touchPosition(0)
{}

//==============================================================================
int32_t UISliderBase::CalculateValue() const
{
  int32_t range = CalculateRange();
  return static_cast<int32_t>(std::round(range * m_percentage));
}

//==============================================================================
void UISliderBase::SetValue( int32_t value )
{
  SetPercentage(static_cast<float>(value) / CalculateRange());
}

//==============================================================================
void UISliderBase::SetPercentage( float percentage )
{
  if (percentage < .0f)
  {
    percentage = .0f;
  }
  else if (percentage > 1.0f)
  {
    percentage = 1.0f;
  }

  float diff = m_percentage - percentage;
  if (diff * diff > std::numeric_limits<float>::epsilon())
  {
    m_percentage = percentage;
    if (onValueChangeCb != 0)
    {
      (*onValueChangeCb)(this, onValueChangeCbData);
    }
  }
}

} // xr

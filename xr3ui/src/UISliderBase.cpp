//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UISliderBase.hpp"
#include <limits>

namespace XR
{

//==============================================================================
UISliderBase::UISliderBase()
: UIImage(),
  pOnValueChangeCb(0),
  pOnValueChangeCbData(0),
  m_percentage(.0f),
  m_isTouched(false),
  m_touchPosition(0)
{}

//==============================================================================
UISliderBase::~UISliderBase()
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

  float diff(m_percentage - percentage);
  if (diff * diff > std::numeric_limits<float>::epsilon())
  {
    m_percentage = percentage;
    if (pOnValueChangeCb != 0)
    {
      (*pOnValueChangeCb)(this, pOnValueChangeCbData);
    }
  }
}

} // XR

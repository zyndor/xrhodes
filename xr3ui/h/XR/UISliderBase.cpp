#include "UISliderBase.hpp"
#include <limits>

namespace XR
{

UISliderBase::UISliderBase()
: UIImage(),
  pOnValueChangeCb(0),
  pOnValueChangeCbData(0),
  m_isTouched(false),
  m_touchPosition(0),
  m_percentage(.0f)
{}

UISliderBase::~UISliderBase()
{}

int UISliderBase::CalculateValue() const
{
  int range(CalculateRange());
  return static_cast<int>(Round(range * m_percentage));
}

void UISliderBase::SetValue( int value )
{
  SetPercentage(static_cast<float>(value) / CalculateRange());
}

void UISliderBase::SetPercentage( float percentage )
{
  if(percentage < .0f)
  {
    percentage = .0f;
  }
  else if(percentage > 1.0f)
  {
    percentage = 1.0f;
  }

  float diff(m_percentage - percentage);
  if(diff * diff > std::numeric_limits<float>::epsilon())
  {
    m_percentage = percentage;
    if(pOnValueChangeCb != 0)
    {
      (*pOnValueChangeCb)(this, pOnValueChangeCbData);
    }
  }
}

} // XR

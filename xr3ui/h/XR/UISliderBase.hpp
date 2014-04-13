//
// Nuclear Heart Games
// XRhodes
//
// UISliderBase.hpp
// @author  Gyorgy Straub
// @date    16/07/2012
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UISLIDERBASE_HPP
#define XR_UISLIDERBASE_HPP

#include "UIImage.hpp"

namespace XR
{

//==============================================================================
class UISliderBase:  public UIImage
{
public:
  // types
  typedef void(*ValueChangedCallback)(UISliderBase* pSlider, void* pVoid);

  // data
  ValueChangedCallback  pOnValueChangeCb;
  void*                 pOnValueChangeCbData;

  Sprite                sliderSprite;

  // structors
  UISliderBase();
  ~UISliderBase();

  // general
  int           GetSliderSpriteWidth() const;
  int           GetSliderSpriteHeight() const;

  float         GetPercentage() const;
  const float&  GetPercentageRef() const;

  int           CalculateValue() const;

  void          SetPercentage( float percentage );
  void          SetValue( int value );

  // virtual
  virtual int   CalculateRange() const =0;

protected:
  // data
  float m_percentage;

  bool  m_isTouched;
  int   m_touchPosition;
};

//==============================================================================
// implementation
//==============================================================================
inline
int UISliderBase::GetSliderSpriteWidth() const
{
  return static_cast<int>(sliderSprite.GetHalfWidth() * 2.0f);
}

//==============================================================================
inline
int UISliderBase::GetSliderSpriteHeight() const
{
  return static_cast<int>(sliderSprite.GetHalfHeight() * 2.0f);
}

//==============================================================================
inline
float UISliderBase::GetPercentage() const
{
  return m_percentage;
}

//==============================================================================
inline
const float& UISliderBase::GetPercentageRef() const
{
  return m_percentage;
}

} // XR

#endif // XR_UISLIDERBASE_HPP

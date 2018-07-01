#ifndef XR_UISLIDERBASE_HPP
#define XR_UISLIDERBASE_HPP
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
///@brief Base class for UIElements whose purpose is to control a normalised,
/// numeric value in the range of [0.0; 1.0] with a sliding handle control.
class UISliderBase:  public UIImage
{
public:
  // types
  using ValueChangedCallback = void(*)(UISliderBase* slider, void* userData);

  // data
  Sprite                sliderSprite;
  Material::Ptr         sliderMaterial; // TODO: UIBuilder support

  ValueChangedCallback  onValueChangeCb;
  void*                 onValueChangeCbData;

  // structors
  UISliderBase();

  // general
  int32_t       GetSliderSpriteWidth() const;
  int32_t       GetSliderSpriteHeight() const;

  float         GetPercentage() const;
  const float&  GetPercentageRef() const;

  int32_t       CalculateValue() const;

  void          SetPercentage( float percentage );
  void          SetValue( int32_t value );

  // virtual
  virtual int32_t CalculateRange() const =0;

protected:
  // data
  float m_percentage;

  bool m_isTouched;
  int32_t m_touchPosition;
};

//==============================================================================
// implementation
//==============================================================================
inline
int32_t UISliderBase::GetSliderSpriteWidth() const
{
  return static_cast<int32_t>(sliderSprite.GetHalfWidth() * 2.0f);
}

//==============================================================================
inline
int32_t UISliderBase::GetSliderSpriteHeight() const
{
  return static_cast<int32_t>(sliderSprite.GetHalfHeight() * 2.0f);
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

} // xr

#endif // XR_UISLIDERBASE_HPP

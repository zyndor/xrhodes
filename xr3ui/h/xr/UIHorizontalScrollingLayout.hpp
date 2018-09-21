#ifndef XR_UIHORIZONTALSCROLLINGLAYOUT_HPP
#define XR_UIHORIZONTALSCROLLINGLAYOUT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "UIHorizontalLayout.hpp"

namespace xr
{

//==============================================================================
///@brief Container that is capable of scrolling its elements horizontally when
/// they don't fit its boundaries, clipping the ones that don't fit.
class UIHorizontalScrollingLayout:  public UIHorizontalLayout
{
public:
  // structors
  UIHorizontalScrollingLayout();

  // general use
  int16_t       GetXOffset() const;
  int16_t&      GetXOffsetRef();
  float         GetSensitivity() const;
  bool          IsTouched() const;

  void          SetXOffset(int16_t xOffs);
  void          SetSensitivity(float s);

  bool  OnMouseAction(const Input::MouseActionData& e) override;
  bool  OnMouseMotion(const Input::MouseMotionData& e) override;

  void  Render(IUIRenderer& renderer) const override;

protected:
  // data
  int16_t m_xOffset;
  float m_sensitivity;

  bool  m_isTouched;
  int16_t m_lastTouchX;

  // internal
  void AlignChildImpl( UIElement* elem ) override;
};

//==============================================================================
// implementation
//==============================================================================
inline
int16_t UIHorizontalScrollingLayout::GetXOffset() const
{
  return m_xOffset;
}

//==============================================================================
inline
int16_t& UIHorizontalScrollingLayout::GetXOffsetRef()
{
  return m_xOffset;
}

//==============================================================================
inline
float UIHorizontalScrollingLayout::GetSensitivity() const
{
  return m_sensitivity;
}

//==============================================================================
inline
bool  UIHorizontalScrollingLayout::IsTouched() const
{
  return m_isTouched;
}

} // xr

#endif // XR_UIHORIZONTALSCROLLINGLAYOUT_HPP

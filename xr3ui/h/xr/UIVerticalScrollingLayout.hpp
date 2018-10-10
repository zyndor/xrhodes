#ifndef XR_UIVERTICALSCROLLINGLAYOUT_HPP
#define XR_UIVERTICALSCROLLINGLAYOUT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "UIVerticalLayout.hpp"

namespace xr
{

//==============================================================================
///@brief Container that is capable of scrolling its elements horizontally when
/// they don't fit its boundaries, clipping the ones that don't fit.
class UIVerticalScrollingLayout:  public UIVerticalLayout
{
public:
  // structors
  UIVerticalScrollingLayout();

  // general use
  int   GetYOffset() const;
  int&  GetYOffsetRef();
  float GetSensitivity() const;
  bool  IsTouched() const;

  void  SetYOffset(int yOffs);
  void  SetSensitivity(float s);

  bool  OnMouseAction(const Input::MouseActionData& e) override;
  bool  OnMouseMotion(const Input::MouseMotionData& e) override;

  void  Render(IUIRenderer& renderer) const override;

protected:
  // data
  int m_yOffset;
  float m_sensitivity;

  bool  m_isTouched;
  int m_lastTouchY;

  // internal
  void AlignChildImpl( UIElement* elem ) override;
};

//==============================================================================
// implementation
//==============================================================================
inline
int UIVerticalScrollingLayout::GetYOffset() const
{
  return m_yOffset;
}

//==============================================================================
inline
int& UIVerticalScrollingLayout::GetYOffsetRef()
{
  return m_yOffset;
}

//==============================================================================
inline
float UIVerticalScrollingLayout::GetSensitivity() const
{
  return m_sensitivity;
}

//==============================================================================
inline
bool  UIVerticalScrollingLayout::IsTouched() const
{
  return m_isTouched;
}

} // xr

#endif // XR_UIVERTICALSCROLLINGLAYOUT_HPP

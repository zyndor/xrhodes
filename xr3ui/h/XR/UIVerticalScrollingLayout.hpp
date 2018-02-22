#ifndef XR_UIVERTICALSCROLLINGLAYOUT_HPP
#define XR_UIVERTICALSCROLLINGLAYOUT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIVerticalLayout.hpp"

namespace XR
{

//==============================================================================
class UIVerticalScrollingLayout:  public UIVerticalLayout
{
public:
  // structors
  UIVerticalScrollingLayout();
  virtual ~UIVerticalScrollingLayout();

  // general use
  int         GetYOffset() const;
  int&        GetYOffsetRef();
  float         GetSensitivity() const;
  bool          IsTouched() const;

  void          SetYOffset(int yOffs);
  void          SetSensitivity(float s);

  bool  OnMouseAction(const Input::MouseActionEvent& e) override;
  bool  OnMouseMotion(const Input::MouseMotionEvent& e) override;

  void  Render(IUIRenderer& renderer) const override;

protected:
  // data
  int m_yOffset;
  float m_sensitivity;

  bool  m_isTouched;
  int m_lastTouchY;

  // internal
  void _AlignElement( UIElement* pElem ) override;
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

} // XR

#endif // XR_UIVERTICALSCROLLINGLAYOUT_HPP

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UIHORIZONTALSCROLLINGLAYOUT_HPP
#define XR_UIHORIZONTALSCROLLINGLAYOUT_HPP

#include "UIHorizontalLayout.hpp"

namespace XR
{

//==============================================================================
class UIHorizontalScrollingLayout:  public UIHorizontalLayout
{
public:
  // structors
  UIHorizontalScrollingLayout();
  virtual ~UIHorizontalScrollingLayout();

  // general use
  int16_t       GetXOffset() const;
  int16_t&      GetXOffsetRef();
  float         GetSensitivity() const;
  bool          IsTouched() const;

  void          SetXOffset(int16_t xOffs);
  void          SetSensitivity(float s);

  virtual bool  OnMouseAction(const Input::MouseActionEvent& e);
  virtual bool  OnMouseMotion(const Input::MouseMotionEvent& e);

  virtual void  Render() const;
  virtual void  Render(UIRenderer* pRenderer) const;

protected:
  // data
  int16_t m_xOffset;
  float m_sensitivity;

  bool  m_isTouched;
  int16_t m_lastTouchX;

  // internal
  virtual void _AlignElement( UIElement* pElem );
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

} // XR

#endif // XR_UIHORIZONTALSCROLLINGLAYOUT_HPP

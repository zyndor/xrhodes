//
// Nuclear Heart Games
// XRhodes
//
// UIHorizontalScrollingLayout.hpp
// @author  Gyorgy Straub
// @date    04/03/2013
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UIHORIZONTALSCROLLINGLAYOUT_HPP
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
  int16         GetXOffset() const;
  int16&        GetXOffsetRef();
  float         GetSensitivity() const;
  bool          IsTouched() const;

  void          SetXOffset(int16 xOffs);
  void          SetSensitivity(float s);

  virtual bool  OnMouseAction(const Input::MouseActionEvent& e);
  virtual bool  OnMouseMotion(const Input::MouseMotionEvent& e);

  virtual void  Render() const;
  virtual void  Render(UIRenderer* pRenderer) const;

protected:
  // data
  int16 m_xOffset;
  float m_sensitivity;

  bool  m_isTouched;
  int16 m_lastTouchX;

  // internal
  virtual void _AlignElement( UIElement* pElem );
};

//==============================================================================
// implementation
//==============================================================================
inline
int16 UIHorizontalScrollingLayout::GetXOffset() const
{
  return m_xOffset;
}

//==============================================================================
inline
int16& UIHorizontalScrollingLayout::GetXOffsetRef()
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

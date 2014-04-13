//
// Nuclear Heart Games
// XRhodes
//
// UIVerticalScrollingLayout.hpp
// @author  Gyorgy Straub
// @date    21/06/2012
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UIVERTICALSCROLLINGLAYOUT_HPP
#define XR_UIVERTICALSCROLLINGLAYOUT_HPP

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
  int16         GetYOffset() const;
  int16&        GetYOffsetRef();
  float         GetSensitivity() const;

  void          SetYOffset(int16 yOffs);
  void          SetSensitivity(float s);

  virtual bool  OnMouseAction(const Input::MouseActionEvent& e);
  virtual bool  OnMouseMotion(const Input::MouseMotionEvent& e);

  virtual void  Render() const;
  virtual void  Render(UIRenderer* pRenderer) const;

protected:
  // data
  int16 m_yOffset;
  float m_sensitivity;

  bool  m_isTouched;
  int16 m_lastTouchY;

  // internal
  virtual void _AlignElement( UIElement* pElem );
};

//==============================================================================
// implementation
//==============================================================================
inline
int16 UIVerticalScrollingLayout::GetYOffset() const
{
  return m_yOffset;
}

//==============================================================================
inline
int16& UIVerticalScrollingLayout::GetYOffsetRef()
{
  return m_yOffset;
}

//==============================================================================
inline
float UIVerticalScrollingLayout::GetSensitivity() const
{
  return m_sensitivity;
}

} // XR

#endif // XR_UIVERTICALSCROLLINGLAYOUT_HPP

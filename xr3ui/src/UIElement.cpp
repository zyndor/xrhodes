//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/UIElement.hpp"
#include "xr/UIEventNotifier.hpp"
#include "xr/Gfx.hpp"

namespace xr
{

//==============================================================================
UIElement::UIElement()
: Rect(0, 0, 0, 0),
  m_parent(nullptr)
{}

//==============================================================================
UIElement::UIElement(int w, int h)
: Rect(0, 0, w, h),
  m_parent(nullptr)
{}

//==============================================================================
UIElement::~UIElement()
{}

//==============================================================================
bool UIElement::OnMouseAction(const Input::MouseActionData& e)
{
  return false;
}

//==============================================================================
bool UIElement::OnMouseMotion(const Input::MouseMotionData& e)
{
  return false;
}

//==============================================================================
void  UIElement::OnChange()
{}

//==============================================================================
void UIElement::SetPosition( int x1, int y1 )
{
  x = x1;
  y = y1;
  OnChange();
}

//==============================================================================
void UIElement::SetSize( int w1, int h1 )
{
  XR_ASSERTMSG(UIElement, w1 >= 0, ("Invalid negative width value."));
  XR_ASSERTMSG(UIElement, h1 >= 0, ("Invalid negative height value."));
  w = w1;
  h = h1;
  OnChange();
}

//==============================================================================
void UIElement::Move( int vx, int vy )
{
  SetPosition(x + vx, y + vy);
}

//==============================================================================
void UIElement::Resize( const UIElement* elem )
{
  XR_ASSERTMSG(UIElement, elem != 0,
    ("Trying to UIElement::Resize() to NULL."));
  SetSize(elem->w, elem->h);
}

//==============================================================================
void UIElement::AlignHorizontally( int x1, Alignment hAlign )
{
  AlignHorizontallyInternal(x1, hAlign);
  OnChange();
}

//==============================================================================
void UIElement::AlignVertically( int y1, Alignment vAlign )
{
  AlignVerticallyInternal(y1, vAlign);
  OnChange();
}

//==============================================================================
void UIElement::Align( int x1, int y1, Alignment hAlign, Alignment vAlign )
{
  AlignHorizontallyInternal(x1, hAlign);
  AlignVerticallyInternal(y1, vAlign);
  OnChange();
}

//==============================================================================
void UIElement::CenterHorizontally()
{
  CenterHorizontallyInternal();
  OnChange();
}

//==============================================================================
void UIElement::CenterVertically()
{
  CenterVerticallyInternal();
  OnChange();
}

//==============================================================================
void UIElement::Center()
{
  CenterHorizontallyInternal();
  CenterVerticallyInternal();
  OnChange();
}

//==============================================================================
void UIElement::CenterHorizontallyInternal()
{
  x = (Gfx::GetLogicalWidth() - w) / 2;
}

//==============================================================================
void UIElement::CenterVerticallyInternal()
{
  y = (Gfx::GetLogicalHeight() - h) / 2;
}

//==============================================================================
void UIElement::AlignHorizontallyInternal( int x1, Alignment hAlign )
{
  switch (hAlign)
  {
  case AL_LOW:
    x = x1;
    break;

  case AL_CENTER:
    x = x1 - w / 2;
    break;

  case AL_HIGH:
    x = x1 - w;
    break;

  case AL_NOALIGN:
    break;
  }
}

//==============================================================================
void UIElement::AlignVerticallyInternal( int y1, Alignment vAlign )
{
  switch (vAlign)
  {
  case AL_LOW:
    y = y1;
    break;

  case AL_CENTER:
    y = y1 - h / 2;
    break;

  case AL_HIGH:
    y = y1 - h;
    break;

  case AL_NOALIGN:
    break;
  }
}

//==============================================================================
void UIElement::OnChangeCaller( void* data )
{
  XR_ASSERTMSG(UIElement, data != nullptr,
    ("Trying to call OnChange() on NULL element."));
  static_cast<UIElement*>(data)->OnChange();
}

//==============================================================================
bool  UIElement::Register( UIEventNotifier& disp )
{
  return disp.AddListener(this);
}

//==============================================================================
bool  UIElement::Unregister( UIEventNotifier& disp )
{
  return disp.RemoveListener(this);
}

//==============================================================================
void UIElement::SetParent( UIContainer* container )
{
  m_parent = container;
}

} // xr

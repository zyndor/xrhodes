#include <XR/Renderer.hpp>
#include "UIElement.hpp"
#include "UIEventDispatcher.hpp"

namespace XR
{

//==============================================================================
UIElement::UIElement()
: Rect(0, 0, 0, 0),
  m_pParent(0)
{}

//==============================================================================
UIElement::UIElement( int16 w, int16 h )
: Rect(0, 0, w, h),
  m_pParent(0)
{}

//==============================================================================
UIElement::~UIElement()
{}

//==============================================================================
bool UIElement::OnMouseAction(const Input::MouseActionEvent& e)
{
  return false;
}

//==============================================================================
bool UIElement::OnMouseMotion(const Input::MouseMotionEvent& e)
{
  return false;
}

//==============================================================================
void  UIElement::OnChange()
{}

//==============================================================================
void UIElement::SetPosition( int16 x1, int16 y1 )
{
  x = x1;
  y = y1;
  OnChange();
}

//==============================================================================
void UIElement::SetSize( int16 w1, int16 h1 )
{
  XR_ASSERTMSG(UIElement, w1 >= 0, ("Invalid negative width value."));
  XR_ASSERTMSG(UIElement, h1 >= 0, ("Invalid negative height value."));
  w = w1;
  h = h1;
  OnChange();
}

//==============================================================================
void UIElement::Move( int16 vx, int16 vy )
{
  SetPosition(x + vx, y + vy);
}

//==============================================================================
void UIElement::Resize( const UIElement* pElem )
{
  XR_ASSERTMSG(UIElement, pElem != 0,
    ("Trying to UIElement::Resize() to NULL."));
  SetSize(pElem->w, pElem->h);
}

//==============================================================================
void UIElement::AlignHorizontally( int16 x1, Alignment hAlign )
{
  _AlignHorizontally(x1, hAlign);
  OnChange();
}

//==============================================================================
void UIElement::AlignVertically( int16 y1, Alignment vAlign )
{
  _AlignVertically(y1, vAlign);
  OnChange();
}

//==============================================================================
void UIElement::Align( int16 x1, int16 y1, Alignment hAlign, Alignment vAlign )
{
  _AlignHorizontally(x1, hAlign);
  _AlignVertically(y1, vAlign);
  OnChange();
}

//==============================================================================
void UIElement::CenterHorizontally()
{
  _CenterHorizontally();
  OnChange();
}

//==============================================================================
void UIElement::CenterVertically()
{
  _CenterVertically();
  OnChange();
}

//==============================================================================
void UIElement::Center()
{
  _CenterHorizontally();
  _CenterVertically();
  OnChange();
}

//==============================================================================
void UIElement::_CenterHorizontally()
{
  x = (Renderer::GetScreenWidth() - w) / 2;
}

//==============================================================================
void UIElement::_CenterVertically()
{
  y = (Renderer::GetScreenHeight() - h) / 2;
}

//==============================================================================
void UIElement::_AlignHorizontally( int16 x1, Alignment hAlign )
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
  }
}

//==============================================================================
void UIElement::_AlignVertically( int16 y1, Alignment vAlign )
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
  }
}

//==============================================================================
void UIElement::OnChangeCaller( void* pData )
{
  XR_ASSERTMSG(UIElement, pData != 0,
    ("Trying to call OnChange() on NULL element."));
  static_cast<UIElement*>(pData)->OnChange();
}

//==============================================================================
bool  UIElement::Register( UIEventDispatcher& disp )
{
  return disp.AddListener(this);
}

//==============================================================================
bool  UIElement::Unregister( UIEventDispatcher& disp )
{
  return disp.RemoveListener(this);
}

//==============================================================================
void UIElement::SetParent( UIContainer* pContainer )
{
  m_pParent = pContainer;
}

} // XR

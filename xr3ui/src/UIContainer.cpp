//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/UIContainer.hpp"
#include "xr/functors.hpp"
#include <algorithm>

namespace xr
{

//==============================================================================
UIContainer::UIContainer()
: UIElement(),
  m_elements()
{}

//==============================================================================
UIContainer::~UIContainer()
{}

//==============================================================================
void UIContainer::Render(IUIRenderer& renderer) const
{
  std::for_each(m_elements.begin(), m_elements.end(),
    Caller<const UIElement, void, IUIRenderer&>(&UIElement::Render, renderer));
}

//==============================================================================
bool  UIContainer::AddElement(UIElement* elem)
{
  XR_ASSERTMSG(UIContainer, elem != 0,
    ("Trying to add NULL element to UIContainer."));
  XR_ASSERTMSG(UIContainer, elem != this,
    ("Trying to add UIContainer to itself."));

  UIElement::List::iterator i0(std::find(m_elements.begin(), m_elements.end(), elem));
  bool  success(i0 == m_elements.end());
  if (success)
  {
    UIContainer*  parent = elem->GetParent();
    if (parent != 0)
    {
      parent->RemoveElement(elem);
    }

    elem->SetParent(this);

    _AddChild(elem);
  }
  //else
  //{
  //  RealignElements(i0);
  //}
  return success;
}

//==============================================================================
bool  UIContainer::RemoveElement(UIElement* elem)
{
  UIElement::List::iterator i0(std::find(m_elements.begin(), m_elements.end(), elem));
  bool  success(i0 != m_elements.end());
  if (success)
  {
    (*i0)->SetParent(0);
    _RealignElements(m_elements.erase(i0));
  }
  return success;
}

//==============================================================================
void UIContainer::_AlignElement(UIElement* elem)
{
  // do some nothings
}

//==============================================================================
void UIContainer::_RealignElements(UIElement::List::iterator i)
{
  UIElement::List el;
  el.splice(el.end(), m_elements, i, m_elements.end());

  for (UIElement::List::iterator i0(el.begin()), i1(el.end()); i0 != i1; ++i0)
  {
    _AddChild(*i0);
  }
}

//==============================================================================
void UIContainer::RemoveAllElements()
{
  for (UIElement::List::iterator i0(m_elements.begin()), i1(m_elements.end()); i0 != i1; ++i0)
  {
    (*i0)->SetParent(0);
  }
  m_elements.clear();
}

//==============================================================================
void UIContainer::_AddChild(UIElement* elem)
{
  _AlignElement(elem);
  m_elements.push_back(elem);
}

//==============================================================================
void UIContainer::OnChange()
{
  _RealignElements(m_elements.begin());
}

//==============================================================================
void UIContainer::SetWidthToContent()
{
  _SetWidthToContent();
  OnChange();
}

//==============================================================================
void UIContainer::SetHeightToContent()
{
  _SetHeightToContent();
  OnChange();
}

//==============================================================================
void UIContainer::SetSizeToContent()
{
  _SetWidthToContent();
  _SetHeightToContent();
  OnChange();
}

//==============================================================================
void UIContainer::_SetWidthToContent()
{
  int wNew(0);
  for (UIElement::List::const_iterator i0(m_elements.begin()), i1(m_elements.end()); i0 != i1; ++i0)
  {
    int wElem((*i0)->w);
    if (wElem > wNew)
    {
      wNew = wElem;
    }
  }

  w = wNew;
}

//==============================================================================
void UIContainer::_SetHeightToContent()
{
  int hNew(0);
  for (UIElement::List::const_iterator i0(m_elements.begin()), i1(m_elements.end()); i0 != i1; ++i0)
  {
    int hElem((*i0)->h);
    if (hElem > hNew)
    {
      hNew = hElem;
    }
  }

  h = hNew;
}

//==============================================================================
bool UIContainer::MoveUpElement(UIElement* elem)
{
  bool  success(RemoveElement(elem));
  if (success)
  {
    _AddChild(elem);
  }
  return success;
}

//==============================================================================
bool UIContainer::OnMouseAction(const Input::MouseActionEvent& e)
{
  bool  isHandled(false);
  for (UIElement::List::iterator i0(m_elements.begin()), i1(m_elements.end());
    i0 != i1; ++i0)
  {
    UIElement*  elem(*i0);
    if (elem->OnMouseAction(e))
    {
      isHandled = true;
      break;
    }
  }
  return isHandled;
}

//==============================================================================
bool UIContainer::OnMouseMotion(const Input::MouseMotionEvent& e)
{
  bool  handled(false);
  for (UIElement::List::iterator i0(m_elements.begin()), i1(m_elements.end());
    i0 != i1; ++i0)
  {
    UIElement*  elem(*i0);
    if (elem->OnMouseMotion(e))
    {
      handled = true;
      break;
    }
  }
  return handled;
}

} // xr

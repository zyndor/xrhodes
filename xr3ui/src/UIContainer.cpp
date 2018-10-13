//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
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
  m_children()
{}

//==============================================================================
void UIContainer::Render(IUIRenderer& renderer) const
{
  std::for_each(m_children.begin(), m_children.end(),
    Caller<const UIElement, void, IUIRenderer&>(&UIElement::Render, renderer));
}

//==============================================================================
bool  UIContainer::AddElement(UIElement* elem)
{
  XR_ASSERTMSG(UIContainer, elem != nullptr,
    ("Trying to add NULL element to UIContainer."));
  XR_ASSERTMSG(UIContainer, elem != this,
    ("Trying to add UIContainer to itself."));

  UIElement::List::iterator i0(std::find(m_children.begin(), m_children.end(), elem));
  bool  success = i0 == m_children.end();
  if (success)
  {
    UIContainer*  parent = elem->GetParent();
    if (parent)
    {
      parent->RemoveElement(elem);
    }

    elem->SetParent(this);

    AddChild(elem);
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
  UIElement::List::iterator i0(std::find(m_children.begin(), m_children.end(), elem));
  bool  success = i0 != m_children.end();
  if (success)
  {
    (*i0)->SetParent(nullptr);
    RealignChildren(m_children.erase(i0));
  }
  return success;
}

//==============================================================================
void UIContainer::AlignChildImpl(UIElement* elem)
{}

//==============================================================================
void UIContainer::RealignChildren(UIElement::List::iterator i)
{
  UIElement::List el;
  el.splice(el.end(), m_children, i, m_children.end());

  for (UIElement::List::iterator i0(el.begin()), i1(el.end()); i0 != i1; ++i0)
  {
    AddChild(*i0);
  }
}

//==============================================================================
void UIContainer::RemoveAllElements()
{
  for (UIElement::List::iterator i0(m_children.begin()), i1(m_children.end());
    i0 != i1; ++i0)
  {
    (*i0)->SetParent(nullptr);
  }
  m_children.clear();
}

//==============================================================================
void UIContainer::AddChild(UIElement* elem)
{
  AlignChildImpl(elem);
  m_children.push_back(elem);
}

//==============================================================================
void UIContainer::OnChange()
{
  RealignChildren(m_children.begin());
}

//==============================================================================
void UIContainer::SetWidthToContent()
{
  SetWidthToContentImpl();
  OnChange();
}

//==============================================================================
void UIContainer::SetHeightToContent()
{
  SetHeightToContentImpl();
  OnChange();
}

//==============================================================================
void UIContainer::SetSizeToContent()
{
  SetWidthToContentImpl();
  SetHeightToContentImpl();
  OnChange();
}

//==============================================================================
void UIContainer::SetWidthToContentImpl()
{
  int wNew = 0;
  for (UIElement::List::const_iterator i0(m_children.begin()), i1(m_children.end()); i0 != i1; ++i0)
  {
    int wElem = (*i0)->w;
    if (wElem > wNew)
    {
      wNew = wElem;
    }
  }

  w = wNew;
}

//==============================================================================
void UIContainer::SetHeightToContentImpl()
{
  int hNew = 0;
  for (UIElement::List::const_iterator i0(m_children.begin()), i1(m_children.end()); i0 != i1; ++i0)
  {
    int hElem = (*i0)->h;
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
  bool  success = RemoveElement(elem);
  if (success)
  {
    AddChild(elem);
  }
  return success;
}

//==============================================================================
bool UIContainer::OnMouseAction(const Input::MouseActionData& e)
{
  bool  isHandled = false;
  for (UIElement::List::iterator i0(m_children.begin()), i1(m_children.end());
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
bool UIContainer::OnMouseMotion(const Input::MouseMotionData& e)
{
  bool  handled(false);
  for (UIElement::List::iterator i0(m_children.begin()), i1(m_children.end());
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

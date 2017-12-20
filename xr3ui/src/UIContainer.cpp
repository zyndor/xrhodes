//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UIContainer.hpp"
#include "XR/functors.hpp"
#include <algorithm>

namespace XR
{

//==============================================================================
UIContainer::UIContainer()
: UIElement(),
  m_lElements()
{}

//==============================================================================
UIContainer::~UIContainer()
{}

//==============================================================================
void UIContainer::Render(IUIRenderer& renderer) const
{
  std::for_each(m_lElements.begin(), m_lElements.end(),
    Caller<const UIElement, void, IUIRenderer&>(&UIElement::Render, renderer));
}

//==============================================================================
bool  UIContainer::AddElement(UIElement* pElem)
{
  XR_ASSERTMSG(UIContainer, pElem != 0,
    ("Trying to add NULL element to UIContainer."));
  XR_ASSERTMSG(UIContainer, pElem != this,
    ("Trying to add UIContainer to itself."));

  UIElement::List::iterator i0(std::find(m_lElements.begin(), m_lElements.end(), pElem));
  bool  success(i0 == m_lElements.end());
  if (success)
  {
    UIContainer*  pParent(pElem->GetParent());
    if (pParent != 0)
    {
      pParent->RemoveElement(pElem);
    }

    pElem->SetParent(this);

    _AddChild(pElem);
  }
  //else
  //{
  //  RealignElements(i0);
  //}
  return success;
}

//==============================================================================
bool  UIContainer::RemoveElement(UIElement* pElem)
{
  UIElement::List::iterator i0(std::find(m_lElements.begin(), m_lElements.end(), pElem));
  bool  success(i0 != m_lElements.end());
  if (success)
  {
    (*i0)->SetParent(0);
    _RealignElements(m_lElements.erase(i0));
  }
  return success;
}

//==============================================================================
void UIContainer::_AlignElement(UIElement* pElem)
{
  // do some nothings
}

//==============================================================================
void UIContainer::_RealignElements(UIElement::List::iterator i)
{
  UIElement::List el;
  el.splice(el.end(), m_lElements, i, m_lElements.end());

  for (UIElement::List::iterator i0(el.begin()), i1(el.end()); i0 != i1; ++i0)
  {
    _AddChild(*i0);
  }
}

//==============================================================================
void UIContainer::RemoveAllElements()
{
  for (UIElement::List::iterator i0(m_lElements.begin()), i1(m_lElements.end()); i0 != i1; ++i0)
  {
    (*i0)->SetParent(0);
  }
  m_lElements.clear();
}

//==============================================================================
void UIContainer::_AddChild(UIElement* pElem)
{
  _AlignElement(pElem);
  m_lElements.push_back(pElem);
}

//==============================================================================
void UIContainer::OnChange()
{
  _RealignElements(m_lElements.begin());
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
  for (UIElement::List::const_iterator i0(m_lElements.begin()), i1(m_lElements.end()); i0 != i1; ++i0)
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
  for (UIElement::List::const_iterator i0(m_lElements.begin()), i1(m_lElements.end()); i0 != i1; ++i0)
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
bool UIContainer::MoveUpElement(UIElement* pElem)
{
  bool  success(RemoveElement(pElem));
  if (success)
  {
    _AddChild(pElem);
  }
  return success;
}

//==============================================================================
bool UIContainer::OnMouseAction(const Input::MouseActionEvent& e)
{
  bool  isHandled(false);
  for (UIElement::List::iterator i0(m_lElements.begin()), i1(m_lElements.end());
    i0 != i1; ++i0)
  {
    UIElement*  pElem(*i0);
    if (pElem->OnMouseAction(e))
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
  for (UIElement::List::iterator i0(m_lElements.begin()), i1(m_lElements.end());
    i0 != i1; ++i0)
  {
    UIElement*  pElem(*i0);
    if (pElem->OnMouseMotion(e))
    {
      handled = true;
      break;
    }
  }
  return handled;
}

} // XR

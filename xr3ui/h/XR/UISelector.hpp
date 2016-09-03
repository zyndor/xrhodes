//
// Nuclear Heart Games
// XRhodes
//
// @author  Gyorgy Straub
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#ifndef XR_UISELECTOR_HPP
#define XR_UISELECTOR_HPP

#include  <map>
#include  "UIElement.hpp"

namespace XR
{

//==============================================================================
class UISelector
{
public:
  // structors
  UISelector();
  ~UISelector();

  // general
  bool        Add(UIElement* pElem, UIElement* pAfter = 0);
  bool        Remove(UIElement* pElem);
  void        Clear();

  UIElement*  GetFirst() const;
  UIElement*  GetLast() const;

  UIElement*  GetPrevious(UIElement* pElem) const;
  UIElement*  GetNext(UIElement* pElem) const;

protected:
  // types
  struct  Entry
  {
    UIElement*  pPrev;
    UIElement*  pNext;
  };

  typedef std::map<UIElement*, Entry>  ElementMap;

  // data
  ElementMap  m_elements;
};

//==============================================================================
// implementation
//==============================================================================
inline
UIElement*  UISelector::GetFirst() const
{
  return m_elements.empty() ? 0 : m_elements.begin()->first;
}

//==============================================================================
inline
UIElement*  UISelector::GetLast() const
{
  return m_elements.empty() ? 0 : m_elements.rbegin()->first;
}

} // XR

#endif  //RB_UISelector_HPP
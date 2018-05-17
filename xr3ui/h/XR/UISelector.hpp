#ifndef XR_UISELECTOR_HPP
#define XR_UISELECTOR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <map>
#include "UIElement.hpp"

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
  bool        Add(UIElement* elem, UIElement* previous = nullptr);
  bool        Remove(UIElement* elem);
  void        Clear();

  UIElement*  GetFirst() const;
  UIElement*  GetLast() const;

  UIElement*  GetPrevious(UIElement* elem) const;
  UIElement*  GetNext(UIElement* elem) const;

protected:
  // types
  struct  Entry
  {
    UIElement*  prev;
    UIElement*  next;
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

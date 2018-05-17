//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UISelector.hpp"

namespace XR
{

//==============================================================================
UISelector::UISelector()
: m_elements()
{}

//==============================================================================
UISelector::~UISelector()
{}

//==============================================================================
bool  UISelector::Add(UIElement* elem, UIElement* previous)
{
  bool  result(previous == nullptr || previous == elem);
  if(result)
  {
    Entry e = { previous, previous };
    m_elements[elem] = e;
  }
  else
  {
    ElementMap::iterator  iFind(m_elements.find(previous));
    result = iFind != m_elements.end();
    if(result)
    {
      UIElement*  next = iFind->second.next;
      if(next != nullptr)
      {
        // if inserting
        ElementMap::iterator  iFindNext(m_elements.find(next));
        XR_ASSERT(UISelector, iFindNext != m_elements.end());

        // replace next's previous with elem
        iFindNext->second.prev = elem;
      }

      // replace previous' next with inserted elem
      iFind->second.next = elem;

      Entry e = { previous, next };
      m_elements[elem] = e;
    }
  }

  return result;
}

//==============================================================================
bool  UISelector::Remove(UIElement* elem)
{
  ElementMap::iterator  iFind(m_elements.find(elem));
  bool  result(iFind != m_elements.end());
  if(result)
  {
    UIElement*  prev = iFind->second.prev;
    UIElement*  next = iFind->second.next;

    ElementMap::iterator  iFindPrev(m_elements.find(prev));
    if(iFindPrev != m_elements.end())
    {
      iFindPrev->second.next = next;
    }

    ElementMap::iterator  iFindNext(m_elements.find(next));
    if(iFindNext != m_elements.end())
    {
      iFindNext->second.prev = prev;
    }

    m_elements.erase(iFind);
  }
  return result;
}

//==============================================================================
void  UISelector::Clear()
{
  m_elements.clear();
}

//==============================================================================
UIElement* UISelector::GetPrevious(UIElement* elem) const
{
  ElementMap::const_iterator  iFind(m_elements.find(elem));
  return iFind != m_elements.end() ? (iFind->second.prev != nullptr ?
    iFind->second.prev : elem) : nullptr;
}

//==============================================================================
UIElement* UISelector::GetNext(UIElement* elem) const
{
  ElementMap::const_iterator  iFind(m_elements.find(elem));
  return iFind != m_elements.end() ? (iFind->second.next != nullptr ?
    iFind->second.next : elem) : nullptr;
}

} // XR

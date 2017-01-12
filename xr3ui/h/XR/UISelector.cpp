//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include  "UISelector.hpp"

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
bool  UISelector::Add(UIElement* pElem, UIElement* pAfter)
{
  bool  result(pAfter == 0 || pAfter == pElem);
  if(result)
  {
    Entry e = { pAfter, pAfter };
    m_elements[pElem] = e;
  }
  else
  {
    ElementMap::iterator  iFind(m_elements.find(pAfter));
    result = iFind != m_elements.end();
    if(result)
    {
      UIElement*  pNext(iFind->second.pNext);
      if(pNext != 0)
      {
        // if inserting
        ElementMap::iterator  iFindNext(m_elements.find(pNext));
        XR_ASSERT(UISelector, iFindNext != m_elements.end());

        // replace next's previous with elem
        iFindNext->second.pPrev = pElem;
      }

      // replace pAfter's next with inserted elem
      iFind->second.pNext = pElem;

      Entry e = { pAfter, pNext };
      m_elements[pElem] = e;
    }
  }

  return result;
}

//==============================================================================
bool  UISelector::Remove(UIElement* pElem)
{
  ElementMap::iterator  iFind(m_elements.find(pElem));
  bool  result(iFind != m_elements.end());
  if(result)
  {
    UIElement*  pPrev(iFind->second.pPrev);
    UIElement*  pNext(iFind->second.pNext);

    ElementMap::iterator  iFindPrev(m_elements.find(pPrev));
    if(iFindPrev != m_elements.end())
    {
      iFindPrev->second.pNext = pNext;
    }

    ElementMap::iterator  iFindNext(m_elements.find(pNext));
    if(iFindNext != m_elements.end())
    {
      iFindNext->second.pPrev = pPrev;
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
UIElement* UISelector::GetPrevious(UIElement* pElem) const
{
  ElementMap::const_iterator  iFind(m_elements.find(pElem));
  return iFind != m_elements.end() ? (iFind->second.pPrev != 0 ? 
    iFind->second.pPrev : pElem) : 0;
}

//==============================================================================
UIElement* UISelector::GetNext(UIElement* pElem) const
{
  ElementMap::const_iterator  iFind(m_elements.find(pElem));
  return iFind != m_elements.end() ? (iFind->second.pNext != 0 ? 
    iFind->second.pNext : pElem) : 0;
}

} // XR
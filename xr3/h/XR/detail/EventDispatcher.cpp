//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "EventDispatcher.hpp"

namespace XR
{

//==============================================================================
EventDispatcherCore::EventDispatcherCore()
: m_listeners(),
  m_isTraversing(false),
  m_postponed()
{}

//==============================================================================
EventDispatcherCore::~EventDispatcherCore()
{
  XR_ASSERT(EventDispatcherCore, !m_isTraversing);
  m_isTraversing = false;
  Clear();
}

//==============================================================================
bool  EventDispatcherCore::RemoveListener(void* pListener) // no ownership transfer
{
  ListenerBaseBase  l(pListener);

  ListenerList::iterator  iFind(GetAdded(&l));
  bool  result(iFind != m_listeners.end());  // && *iErase == pListener;
  if (result)  // if is added
  {
    if (m_isTraversing)  // traversing - postpone removal
    {
      m_postponed.push_back(Postponed(Postponed::REMOVE, pListener));
    }
    else  // not traversing - remove instantly
    {
      delete *iFind;
      m_listeners.erase(iFind);
    }
  }
  else if (m_isTraversing) // not found in added while traversing
  {
    PostponedList::iterator iFindPostponed = GetPostponedAdded(&l);
    result = iFindPostponed != m_postponed.end();
    if (result)  // found it in postponed - remove instantly
    {
      delete static_cast<ListenerBaseBase*>(iFindPostponed->pData);
      m_postponed.erase(iFindPostponed);
    }
  }
  return result;
}

//==============================================================================
void  EventDispatcherCore::ProcessPostponed()
{
  for(PostponedList::iterator i0 = m_postponed.begin(), i1 = m_postponed.end(); i0 != i1; ++i0)
  {
    switch(i0->type)
    {
    case Postponed::ADD:
      _AddListener(static_cast<ListenerBaseBase*>(i0->pData));
      break;

    case Postponed::REMOVE:
      RemoveListener(i0->pData);
      break;

    case Postponed::CLEAR:
      Clear();
      break;
    }
  }
  
  m_postponed.clear();
}

//==============================================================================
void  EventDispatcherCore::Clear()
{
  if (m_isTraversing)
  {
    // clear all postponed adds
    for (PostponedList::iterator i0 = m_postponed.begin(), i1 = m_postponed.end(); i0 != i1;)
    {
      if (i0->type == Postponed::ADD)
      {
        delete static_cast<ListenerBaseBase*>(i0->pData);
      }
    }
    m_postponed.clear();

    // flag postponed clear
    m_postponed.push_back(Postponed(Postponed::CLEAR));
  }
  else
  {
    // clear listeners straight away
    for (ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      delete *i0;
    }
    m_listeners.clear();
  }
}

} // XR

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/detail/EventDispatcher.hpp"

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
      delete static_cast<ListenerBaseBase*>(iFindPostponed->data);
      m_postponed.erase(iFindPostponed);
    }
  }
  return result;
}

//==============================================================================
void  EventDispatcherCore::ProcessPostponed()
{
  for(auto& i: m_postponed)
  {
    switch(i.type)
    {
    case Postponed::ADD:
      AddListenerImpl(static_cast<ListenerBaseBase*>(i.data));
      break;

    case Postponed::REMOVE:
      RemoveListener(i.data);
      break;

    case Postponed::CLEAR:
      Clear();
      break;
    }
  }

  m_postponed.clear();
}

//==============================================================================
void EventDispatcherCore::FinishTraversal()
{
  m_isTraversing = false;

  ProcessPostponed();
}

//==============================================================================
void  EventDispatcherCore::Clear()
{
  if (m_isTraversing)
  {
    // clear all postponed adds
    for (auto& i: m_postponed)
    {
      if (i.type == Postponed::ADD)
      {
        delete static_cast<ListenerBaseBase*>(i.data);
      }
    }
    m_postponed.clear();

    // flag postponed clear
    m_postponed.push_back(Postponed(Postponed::CLEAR));
  }
  else
  {
    // clear listeners straight away
    for (auto& i: m_listeners)
    {
      delete i;
    }
    m_listeners.clear();
  }
}

} // XR

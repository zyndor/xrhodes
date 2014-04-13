//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include  "EventDispatcher.hpp"

namespace XR
{

//==============================================================================
EventDispatcherCore::EventDispatcherCore()
: m_listeners(),
  m_isTraversing(false),
  m_postponedAdd(),
  m_postponedRemove(),
  m_postponedClear(false)
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
  if(result)  // if is added
  {
    if(m_isTraversing)  // traversing - postpone removal
    {
      m_postponedRemove.push_back(pListener);
    }
    else  // not traversing - remove instantly
    {
      delete *iFind;
      m_listeners.erase(iFind);
    }
  }
  else if(m_isTraversing) // not found in added while traversing
  {
    iFind = GetPostponedAdded(&l);
    result = iFind != m_postponedAdd.end();
    if(result)  // found it in postponed - remove instantly
    {
      delete *iFind;
      m_postponedAdd.erase(iFind);
    }
  }
  return result;
}

//==============================================================================
void  EventDispatcherCore::Clear()
{
  if(m_isTraversing)
  {
    // clear all postponed adds
    for(ListenerList::iterator i0(m_postponedAdd.begin()),
      i1(m_postponedAdd.end()); i0 != i1; ++i0)
    {
      delete *i0;
    }
    m_postponedAdd.clear();

    // flag postponed clear
    m_postponedClear = true;
  }
  else
  {
    // clear listeners straight away
    for(ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      delete *i0;
    }
    m_listeners.clear();
  }
}

} // XR

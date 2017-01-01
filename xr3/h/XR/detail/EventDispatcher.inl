//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

// no guard

//==============================================================================
inline
void  EventDispatcherCore::_AddListener(ListenerBaseBase* p)
{
  XR_ASSERT(EventDispatcherCore, !IsAdded(p));
  m_listeners.push_back(p);
}

//==============================================================================
inline
EventDispatcherCore::ListenerList::iterator
EventDispatcherCore::GetAdded(ListenerBaseBase* p)
{
  return std::find_if(m_listeners.begin(), m_listeners.end(),
    ListenerBaseBase::Compare(p));
}

//==============================================================================
inline
EventDispatcherCore::PostponedList::iterator
EventDispatcherCore::GetPostponedAdded(ListenerBaseBase* p)
{
  Postponed  op(Postponed::ADD, p);
  return std::find(m_postponed.begin(), m_postponed.end(), op);
}

//==============================================================================
inline
bool  EventDispatcherCore::IsAdded(ListenerBaseBase* p) const
{
  ListenerList::const_reverse_iterator  iEnd(m_listeners.rend());
  return std::find_if(m_listeners.rbegin(), iEnd,
    ListenerBaseBase::Compare(p)) != iEnd;
}

//==============================================================================
inline
bool  EventDispatcherCore::IsPostponedAdded(ListenerBaseBase* p) const
{
  Postponed  op(Postponed::ADD, p);
  PostponedList::const_reverse_iterator  iEnd(m_postponed.rend());
  return std::find(m_postponed.rbegin(), iEnd, op) != iEnd;
}


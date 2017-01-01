//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_EVENTBROADCASTER_HPP
#define XR_EVENTBROADCASTER_HPP

#include "detail/EventDispatcher.hpp"

namespace XR
{

//==============================================================================
///@brief EventBroadcaster sends an event of type @a E to each of your
/// registered listeners, in the order of their registration.
template <typename E = void>
class EventBroadcaster : protected EventDispatcher<void, E>
{
public:
  // types
  typedef EventDispatcher<void, E>  BaseType;

  // using
  using BaseType::AddListener;
  using BaseType::RemoveListener;
  using BaseType::Clear;

  // general
  void  Broadcast(E eventData)
  {
    // start traversal
    m_isTraversing = true;

    for (ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      static_cast<ListenerBase*>(*i0)->Handle(eventData);
    }

    // finish traversal
    m_isTraversing = false;

    ProcessPostponed();
  }
};

template <>
class EventBroadcaster<void> : protected EventDispatcher<void>
{
public:
  // types
  typedef EventDispatcher<void>  BaseType;

  // using
  using BaseType::AddListener;
  using BaseType::RemoveListener;
  using BaseType::Clear;

  // general
  void  Broadcast()
  {
    // start traversal
    m_isTraversing = true;

    for (ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      static_cast<ListenerBase*>(*i0)->Handle();
    }

    // finish traversal
    m_isTraversing = false;

    ProcessPostponed();
  }
};

} // XR

#endif // XR_EVENTDISPATCHER_HPP
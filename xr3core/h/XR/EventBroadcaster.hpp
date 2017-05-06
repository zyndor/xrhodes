//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_EVENTBROADCASTER_HPP
#define XR_EVENTBROADCASTER_HPP

#include "detail/EventDispatcher.hpp"
#include "ScopeGuard.hpp"

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
    // Start traversal
    m_isTraversing = true;

    // Make sure traversal finishes even if an exception occurs.
    auto traversingGuard = MakeScopeGuard([this]()
    {
      FinishTraversal();
    });

    for (ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      static_cast<ListenerBase*>(*i0)->Handle(eventData);
    }
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
    // Start traversal.
    m_isTraversing = true;

    // Make sure traversal finishes even if an exception occurs.
    auto traversingGuard = MakeScopeGuard([this]()
    {
      FinishTraversal();
    });

    for (ListenerList::iterator i0(m_listeners.begin()), i1(m_listeners.end());
      i0 != i1; ++i0)
    {
      static_cast<ListenerBase*>(*i0)->Handle();
    }
  }
};

} // XR

#endif // XR_EVENTDISPATCHER_HPP
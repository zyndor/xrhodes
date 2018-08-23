#ifndef XR_EVENTBROADCASTER_HPP
#define XR_EVENTBROADCASTER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "detail/EventDispatcher.hpp"
#include "ScopeGuard.hpp"

namespace xr
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
    BaseType::m_isTraversing = true;

    // Make sure traversal finishes even if an exception occurs.
    auto traversingGuard = MakeScopeGuard([this]()
    {
      BaseType::FinishTraversal();
    });

    for (auto& i: BaseType::m_listeners)
    {
      static_cast<typename BaseType::ListenerBase*>(i)->Handle(eventData);
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

    for (auto& i: m_listeners)
    {
      static_cast<ListenerBase*>(i)->Handle();
    }
  }
};

} // xr

#endif // XR_EVENTDISPATCHER_HPP

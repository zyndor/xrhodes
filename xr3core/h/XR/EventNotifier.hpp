#ifndef XR_EVENTNOTIFIER_HPP
#define XR_EVENTNOTIFIER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "detail/EventDispatcher.hpp"
#include "ScopeGuard.hpp"

namespace XR
{

//==============================================================================
///@brief EventNotifier send an event of type @a E to your registered listeners,
/// in the order of their registration, until one of them returns true.
template <typename E = void>
class EventNotifier : protected EventDispatcher<bool, E>
{
public:
  // types
  typedef EventDispatcher<bool, E>  BaseType;

  // using
  using BaseType::AddListener;
  using BaseType::RemoveListener;
  using BaseType::Clear;

  // general
  bool  Notify(E eventData)
  {
    // start traversal
    BaseType::m_isTraversing = true;

    // Make sure traversal finishes even if an exception occurs.
    auto traversingGuard = MakeScopeGuard([this]()
    {
      BaseType::FinishTraversal();
    });

    bool  handled(false);
    for (auto& i: BaseType::m_listeners)
    {
      if (static_cast<typename BaseType::ListenerBase*>(i)->Handle(eventData))
      {
        handled = true;
        break;
      }
    }

    return handled;
  }
};

template <>
class EventNotifier<void> : protected EventDispatcher<bool>
{
public:
  // types
  typedef EventDispatcher<bool>  BaseType;

  // using
  using BaseType::AddListener;
  using BaseType::RemoveListener;
  using BaseType::Clear;

  // general
  bool  Notify()
  {
    // start traversal
    m_isTraversing = true;

    // Make sure traversal finishes even if an exception occurs.
    auto traversingGuard = MakeScopeGuard([this]()
    {
      FinishTraversal();
    });

    bool  handled(false);
    for (auto& i: m_listeners)
    {
      if (static_cast<ListenerBase*>(i)->Handle())
      {
        handled = true;
        break;
      }
    }

    return handled;
  }
};

} // XR

#endif // XR_EVENTNOTIFIER_HPP
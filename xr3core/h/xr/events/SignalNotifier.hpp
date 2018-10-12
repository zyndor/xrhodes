#ifndef XR_SIGNALBROADCASTER_HPP
#define XR_SIGNALBROADCASTER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Signal.hpp"
#include "xr/memory/ScopeGuard.hpp"

namespace xr
{

//==============================================================================
///@brief Provides a facility to call all connected callbacks until the first
/// one returns true.
template <typename... Args>
class SignalNotifier: public Signal<bool, Args...>
{
public:
  // types
  using BaseType = Signal<bool, Args...>;

  // general
  ///@brief Calls each of the connected callbacks, passing @a args to them,
  /// until one of them returns true to indicate that it has handled the
  /// event.
  ///@return Whether any of the connected callbacks has handled the event.
  bool Notify(Args... args)
  {
    SetDispatching(true);
    auto dispatchingGuard = MakeScopeGuard([this] { SetDispatching(false); });

    for (auto& i: BaseType::GetCallbacks())
    {
      if (static_cast<Callback<bool, Args...>&>(*i).Call(args...))
      {
        return true;
      }
    }
    return false;
  }
};

} // xr

#endif //XR_SIGNALBROADCASTER_HPP

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
///@brief Provides a facility to call all connected callbacks with the same
/// arguments, in the order of registration.
template <typename... Args>
class SignalBroadcaster: public Signal<void, Args...>
{
public:
  // types
  using BaseType = Signal<void, Args...>;

  // general
  ///@brief Calls each connected callbacks with @a args.
  void Broadcast(Args... args)
  {
    SetDispatching(true);
    auto dispatchingGuard = MakeScopeGuard([this] { SetDispatching(false); });

    for (auto& i: BaseType::GetCallbacks())
    {
      static_cast<Callback<void, Args...>&>(*i).Call(args...);
    }
  }
};

} // xr

#endif //XR_SIGNALBROADCASTER_HPP

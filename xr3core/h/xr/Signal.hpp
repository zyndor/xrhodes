#ifndef XR_SIGNAL_HPP
#define XR_SIGNAL_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Callback.hpp"
#include <memory>
#include <vector>

namespace xr
{

//==============================================================================
///@brief Core implementation of a signal with registration and unregistration
/// of callbacks, and switching dispatching mode on / off. Dispatching mode means
/// that the list of callbacks is not modified, rather requests to connect and
/// disconnect callbacks are recorded, and these requests are executed once
/// dispatching has finished.
class SignalCore
{
protected:
  // types
  using CallbackPtr = std::unique_ptr<CallbackBase>;
  using CallbackVector = std::vector<CallbackPtr>;

  // internal
  bool ConnectImpl(CallbackBase const& cb);

  bool DisconnectImpl(CallbackBase const& cb);

  CallbackVector& GetCallbacks();

  void SetDispatching(bool state);

private:
  // data
  bool m_isDispatching = false;
  CallbackVector m_callbacks;
  CallbackVector m_toConnect;
  std::vector<CallbackBase*> m_toDisconnect;  // no ownership

  // internal
  bool Find(CallbackBase const& cb, CallbackVector::iterator* iRemove);
  void FinishDispatching();
};

//==============================================================================
template <typename Return, typename... Args>
class Signal : protected SignalCore
{
public:
  // types
  using CallbackType = Callback<Return, Args...>;

  // general
  bool Connect(CallbackType const& cb)
  {
    return SignalCore::ConnectImpl(cb);
  }

  bool Disconnect(CallbackType const& cb)
  {
    return SignalCore::DisconnectImpl(cb);
  }
};

} // xr

#endif //XR_SIGNAL_HPP

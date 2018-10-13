#ifndef XR_UIEVENTNOTIFIER_HPP
#define XR_UIEVENTNOTIFIER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "UIElement.hpp"
#include "xr/Input.hpp"
#include "xr/events/SignalNotifier.hpp"
#include <list>
#include <memory>

namespace xr
{

//==============================================================================
///@brief Handles mouse action and motion events from Input by propagating them
/// to the relevant callback of UIElements registered as listeners. Supports an
/// additional 'zero hit callback' which is triggered when a mouse action goes
/// unhandled by all registered listeners.
class UIEventNotifier
{
public:
  // types
  using ZeroHitCallback = Callback<void, Input::MouseActionData const&>;

  // structors
  UIEventNotifier();

  // general use
  ///@brief Attempts to register for mouse action and motion events.
  ///@return Whether the operation was successful.
  bool  Register();

  ///@brief Attempts to unregister from mouse action and motion events.
  ///@return Whether the operation was successful.
  bool  Unregister();

  ///@brief Sets a function to call in case a mouse action has happened but
  /// was not handled by either of the registered listeners.
  ///@note Transfers ownership of @a onZeroHit.
  void  SetZeroHitCallback(ZeroHitCallback* onZeroHit);

  ///@brief Attempts to add an UIElement to handle mouse action and motion
  /// events.
  ///@return Whether the operation was successful (i.e. element has not yet
  /// been registered).
  bool  AddListener(UIElement* listener);

  ///@brief Attempts to remove the registration of a given UIElement.
  ///@return Whether the operation was successful (i.e. element has been
  /// registered).
  bool  RemoveListener(UIElement* listener);

protected:
  // data
  SignalNotifier<const Input::MouseActionData&> m_actionNotifier;
  SignalNotifier<const Input::MouseMotionData&> m_motionNotifier;

  std::unique_ptr<ZeroHitCallback> m_onZeroHit;

  // internal
  void OnMouseAction(Input::MouseActionData const& e);
  void OnMouseMotion(Input::MouseMotionData const& e);
};

} // xr

#endif // XR_UIEVENTNOTIFIER_HPP

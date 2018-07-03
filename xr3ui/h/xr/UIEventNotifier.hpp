#ifndef XR_UIEVENTNOTIFIER_HPP
#define XR_UIEVENTNOTIFIER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIElement.hpp"
#include "xr/Input.hpp"
#include "xr/EventNotifier.hpp"
#include <list>

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
  using ZeroHitCallback = void(*)(const Input::MouseActionEvent&, void*);

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
  void  SetZeroHitCallback(ZeroHitCallback onZeroHit, void* userData);

  ///@brief Attempts to add an UIElement to handle mouse action and motion
  /// events.
  ///@return Whether the operation was successful (i.e. element has not yet
  /// been registered).
  bool  AddListener(UIElement* listener);

  ///@brief Attempts to remove the registration of a given UIElement.
  ///@return Whether the operation was successful (i.e. element has been
  /// registered).
  bool  RemoveListener(UIElement* listener);

  ///@brief Clearrs the notifier of all listeners.
  void  RemoveAllListeners();

protected:
  // static
  static void UIPointerActionCallback(void* systemData, void* userData);
  static void UIPointerMotionCallback(void* systemData, void* userData);

  // data
  EventNotifier<const Input::MouseActionEvent&> m_actionNotifier;
  EventNotifier<const Input::MouseMotionEvent&> m_motionNotifier;

  ZeroHitCallback m_onZeroHit;
  void*           m_onZeroHitData;
};

} // xr

#endif // XR_UIEVENTNOTIFIER_HPP

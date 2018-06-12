#ifndef XR_UIEVENTNOTIFIER_HPP
#define XR_UIEVENTNOTIFIER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <list>
#include "xr/Input.hpp"
#include "xr/EventNotifier.hpp"
#include "UIElement.hpp"

namespace xr
{

//==============================================================================
class UIEventNotifier
{
public:
  // types
  typedef void(*ZeroHitCallback)(const Input::MouseActionEvent&, void*);

  // structors
  explicit UIEventNotifier();
  ~UIEventNotifier();

  // general use
  bool  Register();
  bool  Unregister();

  void  SetZeroHitCallback(ZeroHitCallback onZeroHit, void* userData);

  bool  AddListener(UIElement* listener);
  bool  AddListenerPriority(UIElement* listener); // TODO: implement or remove
  bool  RemoveListener(UIElement* listener);

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

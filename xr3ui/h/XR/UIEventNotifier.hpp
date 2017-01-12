//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UIEVENTNOTIFIER_HPP
#define XR_UIEVENTNOTIFIER_HPP

#include <list>
#include <XR/Input.hpp>
#include <XR/EventNotifier.hpp>
#include "UIElement.hpp"

namespace XR
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

  void  SetZeroHitCallback(ZeroHitCallback pZeroHitCb, void* pCbData);

  bool  AddListener(UIElement* pListener);
  bool  AddListenerPriority(UIElement* pListener);
  bool  RemoveListener(UIElement* pListener);

  void  RemoveAllListeners();

protected:
  // static
  static void UIPointerActionCallback(void* pSystem, void* pUser);
  static void UIPointerMotionCallback(void* pSystem, void* pUser);

  // data
  EventNotifier<const Input::MouseActionEvent&> m_actionNotifier;
  EventNotifier<const Input::MouseMotionEvent&> m_motionNotifier;

  ZeroHitCallback m_pZeroHitCb;
  void*           m_pZeroHitCbData;
};

} // XR

#endif // XR_UIEVENTNOTIFIER_HPP

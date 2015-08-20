//
// Nuclear Heart Games
// XRhodes
//
// UIEventDispatcher.hpp
// @author  Gyorgy Straub
// @date    25/06/2012
//
// copyright (c) 2011 - 2015. All rights reserved.
//
//==============================================================================
#if !defined XR_UIEVENTDISPATCHER_HPP
#define XR_UIEVENTDISPATCHER_HPP

#include <list>
#include <XR/Input.hpp>
#include <XR/EventDispatcher.hpp>
#include "UIElement.hpp"

namespace XR
{

//==============================================================================
class UIEventDispatcher
{
public:
  // types
  typedef void(*ZeroHitCallback)(const Input::MouseActionEvent&, void*);

  // structors
  explicit UIEventDispatcher();
  ~UIEventDispatcher();

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
  EventDispatcher<const Input::MouseActionEvent&> m_action;
  EventDispatcher<const Input::MouseMotionEvent&> m_motion;

  ZeroHitCallback m_pZeroHitCb;
  void*           m_pZeroHitCbData;
};

} // XR

#endif // XR_UIEVENTDISPATCHER_HPP

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIEventNotifier.hpp"

namespace XR
{

//==============================================================================
void  UIEventNotifier::UIPointerActionCallback(void* pSystem, void* pUser)
{
  UIEventNotifier   *pDispatcher(static_cast<UIEventNotifier*>(pUser));
  Input::MouseActionEvent *pEvent(static_cast<Input::MouseActionEvent*>(pSystem));

  bool  zeroHit(!pDispatcher->m_actionNotifier.Notify(*pEvent) &&
    pDispatcher->m_pZeroHitCb != 0);
  
  if (zeroHit)
  {
    (*pDispatcher->m_pZeroHitCb)(*pEvent, pDispatcher->m_pZeroHitCbData);
  }
}

//==============================================================================
void  UIEventNotifier::UIPointerMotionCallback(void* pSystem, void* pUser)
{
  UIEventNotifier    *pDispatcher(static_cast<UIEventNotifier*>(pUser));
  Input::MouseMotionEvent *pEvent(static_cast<Input::MouseMotionEvent*>(pSystem));

  pDispatcher->m_motionNotifier.Notify(*pEvent);
}

//==============================================================================
UIEventNotifier::UIEventNotifier()
: m_actionNotifier(),
  m_motionNotifier(),
  m_pZeroHitCb(0),
  m_pZeroHitCbData(0)
{}

//==============================================================================
UIEventNotifier::~UIEventNotifier()
{}

//==============================================================================
bool  UIEventNotifier::Register()
{
  return  Input::RegisterCallback(Input::EV_MOUSE_ACTION,
      UIPointerActionCallback, this) &&
    Input::RegisterCallback(Input::EV_MOUSE_MOTION,
      UIPointerMotionCallback, this);
}

//==============================================================================
bool  UIEventNotifier::Unregister()
{
  return  Input::UnregisterCallback(Input::EV_MOUSE_ACTION,
      UIPointerActionCallback) &&
    Input::UnregisterCallback(Input::EV_MOUSE_MOTION,
      UIPointerMotionCallback);
}

//==============================================================================
bool  UIEventNotifier::AddListener(UIElement* pListener)
{
  m_actionNotifier.AddListener(pListener, &UIElement::OnMouseAction);
  m_motionNotifier.AddListener(pListener, &UIElement::OnMouseMotion);
  return true;
}

//==============================================================================
bool  UIEventNotifier::RemoveListener(UIElement* pListener)
{
  m_actionNotifier.RemoveListener(pListener);
  m_motionNotifier.RemoveListener(pListener);
  return true;
}

//==============================================================================
void UIEventNotifier::RemoveAllListeners()
{
  m_actionNotifier.Clear();
  m_motionNotifier.Clear();
}

//==============================================================================
void UIEventNotifier::SetZeroHitCallback(ZeroHitCallback pZeroHitCb, void* pCbData)
{
  m_pZeroHitCb = pZeroHitCb;
  m_pZeroHitCbData = pCbData;
}

} // XR
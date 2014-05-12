#include "UIEventDispatcher.hpp"

namespace XR
{

//==============================================================================
void  UIEventDispatcher::UIPointerActionCallback( void* pSystem, void* pUser )
{
  UIEventDispatcher   *pDispatcher(static_cast<UIEventDispatcher*>(pUser));
  Input::MouseActionEvent *pEvent(static_cast<Input::MouseActionEvent*>(pSystem));

  bool  zeroHit(!pDispatcher->m_action.Notify(*pEvent) &&
    pDispatcher->m_pZeroHitCb != 0);
  
  if (zeroHit)
  {
    (*pDispatcher->m_pZeroHitCb)(*pEvent, pDispatcher->m_pZeroHitCbData);
  }
}

//==============================================================================
void  UIEventDispatcher::UIPointerMotionCallback( void* pSystem, void* pUser )
{
  UIEventDispatcher    *pDispatcher(static_cast<UIEventDispatcher*>(pUser));
  Input::MouseMotionEvent *pEvent(static_cast<Input::MouseMotionEvent*>(pSystem));

  pDispatcher->m_motion.Notify(*pEvent);
}

//==============================================================================
UIEventDispatcher::UIEventDispatcher()
: m_action(),
  m_motion(),
  m_pZeroHitCb(0),
  m_pZeroHitCbData(0)
{}

//==============================================================================
UIEventDispatcher::~UIEventDispatcher()
{}

//==============================================================================
bool  UIEventDispatcher::Register()
{
  return  Input::RegisterCallback(Input::EV_MOUSE_ACTION,
      UIPointerActionCallback, this) &&
    Input::RegisterCallback(Input::EV_MOUSE_MOTION,
      UIPointerMotionCallback, this);
}

//==============================================================================
bool  UIEventDispatcher::Unregister()
{
  return  Input::UnregisterCallback(Input::EV_MOUSE_ACTION,
      UIPointerActionCallback) &&
    Input::UnregisterCallback(Input::EV_MOUSE_MOTION,
      UIPointerMotionCallback);
}

//==============================================================================
bool  UIEventDispatcher::AddListener(UIElement* pListener)
{
  m_action.AddListener(pListener, &UIElement::OnMouseAction);
  m_motion.AddListener(pListener, &UIElement::OnMouseMotion);
  return true;
}

//==============================================================================
bool  UIEventDispatcher::RemoveListener(UIElement* pListener)
{
  m_action.RemoveListener(pListener);
  m_motion.RemoveListener(pListener);
  return true;
}

//==============================================================================
void UIEventDispatcher::RemoveAllListeners()
{
  m_action.Clear();
  m_motion.Clear();
}

//==============================================================================
void UIEventDispatcher::SetZeroHitCallback( ZeroHitCallback pZeroHitCb, void* pCbData )
{
  m_pZeroHitCb = pZeroHitCb;
  m_pZeroHitCbData = pCbData;
}

} // XR
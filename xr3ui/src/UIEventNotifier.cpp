//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/UIEventNotifier.hpp"

namespace xr
{

//==============================================================================
void  UIEventNotifier::UIPointerActionCallback(void* systemData, void* userData)
{
  UIEventNotifier   *dispatcher(static_cast<UIEventNotifier*>(userData));
  Input::MouseActionEvent *event(static_cast<Input::MouseActionEvent*>(systemData));

  bool  zeroHit(!dispatcher->m_actionNotifier.Notify(*event) &&
    dispatcher->m_onZeroHit);
  if (zeroHit)
  {
    (*dispatcher->m_onZeroHit)(*event, dispatcher->m_onZeroHitData);
  }
}

//==============================================================================
void  UIEventNotifier::UIPointerMotionCallback(void* systemData, void* userData)
{
  UIEventNotifier    *dispatcher(static_cast<UIEventNotifier*>(userData));
  Input::MouseMotionEvent *event(static_cast<Input::MouseMotionEvent*>(systemData));

  dispatcher->m_motionNotifier.Notify(*event);
}

//==============================================================================
UIEventNotifier::UIEventNotifier()
: m_actionNotifier(),
  m_motionNotifier(),
  m_onZeroHit(nullptr),
  m_onZeroHitData(nullptr)
{}

//==============================================================================
UIEventNotifier::~UIEventNotifier()
{}

//==============================================================================
bool  UIEventNotifier::Register()
{
  return  Input::RegisterCallback(Input::Event::MouseAction,
      UIPointerActionCallback, this) &&
    Input::RegisterCallback(Input::Event::MouseMotion,
      UIPointerMotionCallback, this);
}

//==============================================================================
bool  UIEventNotifier::Unregister()
{
  return  Input::UnregisterCallback(Input::Event::MouseAction,
      UIPointerActionCallback) &&
    Input::UnregisterCallback(Input::Event::MouseMotion,
      UIPointerMotionCallback);
}

//==============================================================================
bool  UIEventNotifier::AddListener(UIElement* listener)
{
  m_actionNotifier.AddListener(*listener, &UIElement::OnMouseAction);
  m_motionNotifier.AddListener(*listener, &UIElement::OnMouseMotion);
  return true;
}

//==============================================================================
bool  UIEventNotifier::RemoveListener(UIElement* listener)
{
  m_actionNotifier.RemoveListener(listener);
  m_motionNotifier.RemoveListener(listener);
  return true;
}

//==============================================================================
void UIEventNotifier::RemoveAllListeners()
{
  m_actionNotifier.Clear();
  m_motionNotifier.Clear();
}

//==============================================================================
void UIEventNotifier::SetZeroHitCallback(ZeroHitCallback onZeroHit, void* userData)
{
  m_onZeroHit = onZeroHit;
  m_onZeroHitData = userData;
}

} // xr

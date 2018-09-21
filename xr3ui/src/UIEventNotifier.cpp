//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/UIEventNotifier.hpp"

namespace xr
{

//==============================================================================
UIEventNotifier::UIEventNotifier()
: m_actionNotifier(),
  m_motionNotifier(),
  m_onZeroHit(nullptr)
{}

//==============================================================================
bool  UIEventNotifier::Register()
{
  return  Input::MouseActionSignal().Connect(
    MakeCallback(*this, &UIEventNotifier::OnMouseAction)) &&
    Input::MouseMotionSignal().Connect(
      MakeCallback(*this, &UIEventNotifier::OnMouseMotion));
}

//==============================================================================
bool  UIEventNotifier::Unregister()
{
  return  Input::MouseActionSignal().Disconnect(
    MakeCallback(*this, &UIEventNotifier::OnMouseAction)) &&
    Input::MouseMotionSignal().Disconnect(
      MakeCallback(*this, &UIEventNotifier::OnMouseMotion));
}

//==============================================================================
bool  UIEventNotifier::AddListener(UIElement* listener)
{
  return m_actionNotifier.Connect(
    MakeCallback(*listener, &UIElement::OnMouseAction)) ||
    m_motionNotifier.Connect(
      MakeCallback(*listener, &UIElement::OnMouseMotion));
}

//==============================================================================
bool  UIEventNotifier::RemoveListener(UIElement* listener)
{
  return m_actionNotifier.Disconnect(
    MakeCallback(*listener, &UIElement::OnMouseAction)) ||
    m_motionNotifier.Disconnect(
      MakeCallback(*listener, &UIElement::OnMouseMotion));
}

//==============================================================================
void UIEventNotifier::SetZeroHitCallback(ZeroHitCallback* onZeroHit)
{
  m_onZeroHit.reset(onZeroHit);
}

//==============================================================================
void  UIEventNotifier::OnMouseAction(Input::MouseActionData const& e)
{
  bool zeroHit = !m_actionNotifier.Notify(e) && m_onZeroHit;
  if (zeroHit)
  {
    m_onZeroHit->Call(e);
  }
}

//==============================================================================
void  UIEventNotifier::OnMouseMotion(Input::MouseMotionData const& e)
{
  m_motionNotifier.Notify(e);
}

} // xr

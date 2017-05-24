//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/MouseHandler.hpp>

namespace XR
{

//==============================================================================
MouseHandler::MouseHandler()
{}

//==============================================================================
MouseHandler::~MouseHandler()
{}

//==============================================================================
void  MouseHandler::RegisterMouseCallbacks()
{
  Input::RegisterCallback(Input::EV_MOUSE_ACTION,
    CallbackAdaptor<const Input::MouseActionEvent&>::Function<MouseHandler, &MouseHandler::OnMouseAction>,
    this);
  Input::RegisterCallback(Input::EV_MOUSE_MOTION,
    CallbackAdaptor<const Input::MouseMotionEvent&>::Function<MouseHandler, &MouseHandler::OnMouseMotion>,
    this);
}

//==============================================================================
void  MouseHandler::UnregisterMouseCallbacks()
{
  Input::UnregisterCallback(Input::EV_MOUSE_MOTION,
    CallbackAdaptor<const Input::MouseMotionEvent&>::Function<MouseHandler, &MouseHandler::OnMouseMotion>);
  Input::UnregisterCallback(Input::EV_MOUSE_ACTION,
    CallbackAdaptor<const Input::MouseActionEvent&>::Function<MouseHandler, &MouseHandler::OnMouseAction>);
}

} // XR

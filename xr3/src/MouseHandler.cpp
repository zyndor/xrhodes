//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/MouseHandler.hpp"

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
  Input::RegisterCallback(Input::Event::MouseAction,
    CallbackAdaptor<const Input::MouseActionEvent&>::Function<MouseHandler, &MouseHandler::OnMouseAction>,
    this);
  Input::RegisterCallback(Input::Event::MouseMotion,
    CallbackAdaptor<const Input::MouseMotionEvent&>::Function<MouseHandler, &MouseHandler::OnMouseMotion>,
    this);
}

//==============================================================================
void  MouseHandler::UnregisterMouseCallbacks()
{
  Input::UnregisterCallback(Input::Event::MouseMotion,
    CallbackAdaptor<const Input::MouseMotionEvent&>::Function<MouseHandler, &MouseHandler::OnMouseMotion>);
  Input::UnregisterCallback(Input::Event::MouseAction,
    CallbackAdaptor<const Input::MouseActionEvent&>::Function<MouseHandler, &MouseHandler::OnMouseAction>);
}

} // XR

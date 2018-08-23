//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/MouseHandler.hpp"

namespace xr
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

} // xr

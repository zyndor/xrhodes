//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include  "MouseHandler.hpp"

namespace XR
{

//==============================================================================
void  MouseHandler::_OnMouseAction(void* pSystem, void* pUser)
{
  static_cast<MouseHandler*>(pUser)->
    OnMouseAction(*static_cast<Input::MouseActionEvent*>(pSystem));
}

//==============================================================================
void  MouseHandler::_OnMouseMotion(void* pSystem, void* pUser)
{
  static_cast<MouseHandler*>(pUser)->
    OnMouseMotion(*static_cast<Input::MouseMotionEvent*>(pSystem));
}

//==============================================================================
MouseHandler::MouseHandler()
{}

//==============================================================================
MouseHandler::~MouseHandler()
{}

//==============================================================================
void  MouseHandler::RegisterMouseCallbacks()
{
  Input::RegisterCallback(Input::EV_MOUSE_ACTION, _OnMouseAction, this);
  Input::RegisterCallback(Input::EV_MOUSE_MOTION, _OnMouseMotion, this);
}

//==============================================================================
void  MouseHandler::UnregisterMouseCallbacks()
{
  Input::UnregisterCallback(Input::EV_MOUSE_ACTION, _OnMouseAction);
  Input::UnregisterCallback(Input::EV_MOUSE_MOTION, _OnMouseMotion);
}

} // XR

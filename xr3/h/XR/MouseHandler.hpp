//
// Nuclear Heart Games
// XRhodes
//
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
//
// copyright (c) 2011 - 2016. All rights reserved.
//
//==============================================================================
#ifndef XR_MOUSEHANDLER_HPP
#define XR_MOUSEHANDLER_HPP

#include  "Input.hpp"

namespace XR
{

//==============================================================================
class MouseHandler
{
public:
  // structors
  MouseHandler();
  virtual ~MouseHandler();

  // general
  void  RegisterMouseCallbacks();
  void  UnregisterMouseCallbacks();

  // virtual
  virtual void  OnMouseAction(const Input::MouseActionEvent& e) =0;
  virtual void  OnMouseMotion(const Input::MouseMotionEvent& e) =0;
};

} // XR

#endif // XR_MOUSEHANDLER_HPP

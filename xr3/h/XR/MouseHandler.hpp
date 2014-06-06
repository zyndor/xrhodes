//
// Nuclear Heart Games
// XRhodes
//
// MouseHandler.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    06/06/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
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

private:
  // static
  static void _OnMouseAction(void* pSystem, void* pUser);
  static void _OnMouseMotion(void* pSystem, void* pUser);
};

} // XR

#endif // XR_MOUSEHANDLER_HPP

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_MOUSEHANDLER_HPP
#define XR_MOUSEHANDLER_HPP

#include "Input.hpp"

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

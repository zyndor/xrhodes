#ifndef XR_MOUSEHANDLER_HPP
#define XR_MOUSEHANDLER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "Input.hpp"

namespace xr
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

} // xr

#endif // XR_MOUSEHANDLER_HPP

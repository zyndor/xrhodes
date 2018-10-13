#ifndef XR_SCISSORSTACK_HPP
#define XR_SCISSORSTACK_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Rect.hpp"
#include "xr/types/fundamentals.hpp"

namespace xr
{

//==============================================================================
///@brief Provides convenience for storing a history of rectangles used for
/// scissor testing, and switching back to previous ones without requiring
/// client code to have access to them.
class ScissorStack
{
  XR_NONOBJECT_DECL(ScissorStack)

public:
  ///@brief ScissorStack Push-Pop frame that cleans itself up.
  struct Scope
  {
    Scope(Rect const* rect, bool intersect);
    ~Scope();
  };

  // static
  ///@brief Initializes the ScissorStack.
  static void Init();

  ///@brief Initializes the ScissorStack only if it has not been yet.
  static void RequireInit();

  ///@brief Sets the scissor rectangle and stores it on top of the stack.
  ///@note No ownership transfer. The rectangles are assumed to outlive the
  /// call to Pop() that is supposed to remove them.
  static void Push(Rect const* rect, bool intersect);

  ///@return The current scissor rect known to the stack, including intersections
  /// where requested.
  static Rect const* GetCurrent();

  ///@brief Removes the rectangle on the top of the stack (there must be one),
  /// and sets the one on the new top of the stack (or disables scissor
  /// testing if the stack is now empty).
  static void Pop();
};

};

#endif //XR_SCISSORSTACK_HPP

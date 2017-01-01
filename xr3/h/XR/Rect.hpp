//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_RECT_HPP
#define XR_RECT_HPP

#include "types.hpp"

namespace XR
{
  
//==============================================================================
class Rect
{
public:
  // data
  int16 x, y, w, h;

  // structors
  Rect()
  : x(0), y(0), w(0), h(0)
  {}

  Rect(int16 x_, int16 y_, int16 w_, int16 h_)
  : x(x_), y(y_), w(w_), h(h_)
  {}
};

} // XR

#endif  //XR_RECT_HPP
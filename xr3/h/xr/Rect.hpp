#ifndef XR_RECT_HPP
#define XR_RECT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <cstdint>

namespace xr
{

//==============================================================================
class Rect
{
public:
  // data
  int32_t x, y, w, h;

  // structors
  Rect()
  : x(0), y(0), w(0), h(0)
  {}

  Rect(int32_t x_, int32_t y_, int32_t w_, int32_t h_)
  : x(x_), y(y_), w(w_), h(h_)
  {}
};

} // XR

#endif  //XR_RECT_HPP
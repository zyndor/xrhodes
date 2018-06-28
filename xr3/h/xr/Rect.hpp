#ifndef XR_RECT_HPP
#define XR_RECT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <algorithm>
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

  // general
  ///@brief Whether any of the dimensions of this is zero.
  bool IsZero() const
  {
    return w == 0 || h == 0;
  }

  ///@return A rectangle that the common area between this and @a rhs encloses.
  Rect Intersection(Rect const& rhs) const
  {
    Rect result(std::max(x, rhs.x), std::max(y, rhs.y), 0, 0);
    result.w = std::max(std::min(x + w, rhs.x + rhs.w) - result.x, 0);
    result.h = std::max(std::min(y + h, rhs.y + rhs.h) - result.y, 0);
    return result;
  }

  ///@return A rectangle that contains both this and @a rhs.
  Rect  Union(Rect const& rhs) const
  {
    Rect result(std::min(x, rhs.x), std::min(y, rhs.y), 0, 0);
    result.w = std::max(x + w, rhs.x + rhs.w) - result.x;
    result.h = std::max(y + h, rhs.y + rhs.h) - result.y;
    return result;
  }
};

} // XR

#endif  //XR_RECT_HPP

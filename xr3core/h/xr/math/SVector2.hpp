#ifndef XR_SVECTOR2_HPP
#define XR_SVECTOR2_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <cstdint>
#include <cmath>

namespace xr
{

//==============================================================================
///@brief Signed short integer vector type.
class SVector2
{
public:
  // static
  inline
  static constexpr SVector2  Zero()
  {
    return SVector2(0, 0);
  }

  // data
  int16_t x, y;

  // structors
  constexpr SVector2()
  : SVector2{ 0, 0 }
  {}

  constexpr SVector2(int16_t x_, int16_t y_)
  : x{ x_ },
    y{ y_ }
  {}

  explicit constexpr SVector2(int16_t const data[2])
  : x{ data[0] },
    y{ data[1] }
  {}

  // general
  float Magnitude() const
  {
    return std::sqrt(Dot());
  }

  float Dot() const
  {
    return Dot(*this);
  }

  float Dot(const SVector2& rhs) const
  {
    return static_cast<float>(x * rhs.x + y * rhs.y);
  }

  // range based for support
  int16_t* begin()
  {
    return &x;
  }

  int16_t* end()
  {
    return &x + 2;
  }

  int16_t const* begin() const
  {
    return &x;
  }

  int16_t const* end() const
  {
    return &x + 2;
  }

  // operators
  SVector2& operator +=(const SVector2& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  SVector2  operator +(const SVector2& rhs) const
  {
    SVector2  temp(rhs);
    temp += *this;
    return temp;
  }

  SVector2& operator -=(const SVector2& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  SVector2  operator -(const SVector2& rhs) const
  {
    SVector2  temp(*this);
    temp -= rhs;
    return temp;
  }

  SVector2 operator -() const
  {
    return SVector2(-x, -y);
  }
};

}

#define XR_TRACE_SVECTOR2(chnl, vector)\
{\
  xr::SVector2 const& v = (vector);\
  XR_TRACE(chnl, ("%s { %d, %d }", #vector, v.x, v.y));\
}\

#endif  //XR_SVECTOR2_HPP

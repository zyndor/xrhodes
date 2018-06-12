//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_SVECTOR2_HPP
#define XR_SVECTOR2_HPP

#include <cstdint>
#include <cmath>

namespace xr
{

//==============================================================================
struct SVector2
{
  // static
  inline
  static SVector2  Zero()
  {
    return SVector2(0, 0);
  }

  // data
  union
  {
    struct
    {
      int16_t x, y;
    };
    int16_t data[2];
  };

  // structors
  SVector2()
  : x(0), y(0)
  {}

  SVector2(int16_t x_, int16_t y_)
  : x(x_), y(y_)
  {}

  explicit SVector2(const int16_t data_[2])
  : x(data_[0]),
    y(data_[1])
  {}

  // general
  int16_t Magnitude() const
  {
    return int16_t(std::ceil(std::sqrt(Dot())));
  }

  int16_t Dot() const
  {
    return Dot(*this);
  }

  int16_t Dot(const SVector2& rhs) const
  {
    return x * rhs.x + y * rhs.y;
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
    SVector2  temp(rhs);
    temp -= *this;
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

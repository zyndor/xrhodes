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

namespace XR
{

//==============================================================================
struct SVector2
{
  // static
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
    int16_t arData[2];
  };

  // structors
  SVector2()
  : x(0), y(0)
  {}

  SVector2(int16_t x_, int16_t y_)
  : x(x_), y(y_)
  {}

  explicit SVector2(const int16_t parData[2])
  : x(parData[0]),
    y(parData[1])
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


#define XR_TRACE_SVECTOR2(name, vector)  XR_TRACE(name, ("{ %d, %d }",\
  name, vector.x, vector.y))

#endif  //XR_SVECTOR2_HPP
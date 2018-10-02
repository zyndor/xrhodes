#ifndef XR_VECTOR2_HPP
#define XR_VECTOR2_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/debug.hpp"
#include <cstdint>
#include <cmath>

namespace xr
{

//==============================================================================
class Vector2
{
public:
  // types
  enum
  {
    X,
    Y,
    kNumComponents
  };

  // static
  inline
  static Vector2  Zero()
  {
    return Vector2(0.f, 0.f);
  }

  inline
  static Vector2  One()
  {
    return Vector2(1.f, 1.f);
  }

  inline
  static Vector2  UnitX()
  {
    return Vector2(1.f, 0.f);
  }

  inline
  static Vector2  UnitY()
  {
    return Vector2(0.f, 1.f);
  }

  // data
  union
  {
    struct
    {
      float x, y;
    };
    float data[kNumComponents];
  };

  // structors
  Vector2()
  : Vector2(0.f, 0.f)
  {}

  Vector2(float x_, float y_)
  : x{ x_ },
    y{ y_ }
  {}

  explicit Vector2(const float data_[kNumComponents])
  : x{ data_[X] },
    y{ data_[Y] }
  {}

  // general
  ///@brief Calculates the magnitude of this vector.
  float Magnitude() const
  {
    return std::sqrt(Dot());
  }

  ///@brief Calculates the dot product of this vector with itself.
  float Dot() const
  {
    return Dot(*this);
  }

  ///@brief Calculates the dot product of this vector with @a rhs.
  float Dot(Vector2 const& rhs) const
  {
    return x * rhs.x + y * rhs.y;
  }

  ///@brief Scales the vector so that its length is @a length.
  ///@note This vector must be non-zero.
  Vector2& Normalise(float length = 1.f)
  {
    float d = Dot();
    XR_ASSERT(Vector2, d > 0.f);
    return this->operator*=(length / std::sqrt(d));
  }

  //@return A copy of this vector, normalised to @a length.
  ///@note This vector must be non-zero.
  Vector2 Normalised(float length = 1.f) const
  {
    Vector2 result(*this);
    return result.Normalise(length);
  }

  ///@brief Pseudo-cross product - calculates the dot product of this
  /// vector with the perpendicular of @a rhs.
  float Cross(Vector2 const& rhs) const
  {
    return x * rhs.y - y * rhs.x;
  }

  ///@brief Linearly interpolates between this vector and @a to, at the given
  /// blend factor @a t.
  Vector2 Lerp(Vector2 const& to, float t) const
  {
    return Vector2(x + (to.x - x) * t, y + (to.y - y) * t);
  }

  ///@brief Returns a conversion of this vector into polar coordinates.
  Vector2 ToPolar() const
  {
    return Vector2(atan2f(y, x), Magnitude());
  }

  ///@brief Returns a conversion of this vector into cartesian coordinates.
  Vector2 ToCartesian() const
  {
    return Vector2(cosf(x) * y, sinf(x) * y);
  }

  // operators
  Vector2& operator +=(Vector2 const& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  Vector2  operator +(Vector2 const& rhs) const
  {
    return Vector2(x + rhs.x, y + rhs.y);
  }

  Vector2& operator -=(Vector2 const& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  Vector2  operator -(Vector2 const& rhs) const
  {
    return Vector2(x - rhs.x, y - rhs.y);
  }

  Vector2 operator -() const
  {
    return Vector2(-x, -y);
  }

  Vector2&  operator *=(float s)
  {
    x *= s;
    y *= s;
    return *this;
  }

  Vector2  operator *(float s) const
  {
    return Vector2(x * s, y * s);
  }

  Vector2&  operator /=(float s)
  {
    XR_ASSERT(Vector2, s > 0.f);
    s = 1.f / s;
    x *= s;
    y *= s;
    return *this;
  }

  Vector2  operator /(float s) const
  {
    XR_ASSERT(Vector2, s * s > 0.f);
    s = 1.f / s;
    return Vector2(x * s, y * s);
  }

  Vector2&  operator*=(Vector2 const& rhs)
  {
    x *= rhs.x;
    y *= rhs.y;
    return *this;
  }

  Vector2 operator*(Vector2 const& rhs) const
  {
    Vector2 temp(*this);
    temp *= rhs;
    return temp;
  }

  Vector2&  operator/=(Vector2 const& rhs)
  {
    XR_ASSERT(Vector2, rhs.x * rhs.x > 0.f);
    x /= rhs.x;
    XR_ASSERT(Vector2, rhs.y * rhs.y > 0.f);
    y /= rhs.y;
    return *this;
  }

  Vector2 operator/(Vector2 const& rhs) const
  {
    Vector2 temp(*this);
    temp /= rhs;
    return temp;
  }
};

}

#define XR_TRACE_VECTOR2(chnl, vector)\
{\
  xr::Vector2 const& v = (vector);\
  XR_TRACE(chnl, ("%s { %.3f, %.3f }", #vector, v.x, v.y));\
}\

#endif //XR_VECTOR2_HPP

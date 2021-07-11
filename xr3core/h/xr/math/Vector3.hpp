#ifndef XR_VECTOR3_HPP
#define XR_VECTOR3_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/math/Vector2.hpp"
#include "xr/debug.hpp"
#include <cstdint>
#include <cmath>

namespace xr
{

//==============================================================================
class Vector3
{
public:
  // types
  enum
  {
    X,
    Y,
    Z,
    kNumComponents
  };

  // static
  inline
  static constexpr Vector3  Zero()
  {
    return Vector3(0.f, 0.f, 0.f);
  }

  inline
  static constexpr Vector3  One()
  {
    return Vector3(1.f, 1.f, 1.f);
  }

  inline
  static constexpr Vector3  UnitX()
  {
    return Vector3(1.f, 0.f, 0.f);
  }

  inline
  static constexpr Vector3  UnitY()
  {
    return Vector3(0.f, 1.f, 0.f);
  }

  inline
  static constexpr Vector3  UnitZ()
  {
    return Vector3(0.f, 0.f, 1.f);
  }

  ///@brief Converts a normal, packed into a Vector2, to a Vector3.
  static Vector3 UnpackNormal(Vector2 const& v)
  {
    return Vector3(v.x, v.y, 1.f - v.Magnitude());
  }

  // data
  float  x, y, z;

  // structors
  constexpr Vector3()
  : Vector3{ 0.f, 0.f, 0.f }
  {}

  constexpr Vector3(float x_, float y_, float z_)
  : x{ x_ },
    y{ y_ },
    z{ z_ }
  {}

  explicit constexpr Vector3(float const data[kNumComponents])
  : x{ data[X] },
    y{ data[Y] },
    z{ data[Z] }
  {}

  // general
  ///@brief Calculates the dot product of this vector with @a rhs.
  float Dot(Vector3 const& rhs) const
  {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }

  ///@brief Calculates the dot product of this vector with itself.
  float Dot() const
  {
    return Dot(*this);
  }

  ///@brief Calculates the magnitude of this vector.
  float Magnitude() const
  {
    return std::sqrt(Dot());
  }

  ///@brief Scales the vector so that its length is @a length.
  ///@note This vector must be non-zero.
  Vector3& Normalise(float length = 1.f)
  {
    float d = Dot();
    XR_ASSERT(Vector3, d > 0.f);
    return this->operator*=(length / std::sqrt(d));
  }

  //@return A copy of this vector, normalised to @a length.
  ///@note This vector must be non-zero.
  Vector3 Normalised(float length = 1.f) const
  {
    Vector3 result(*this);
    return result.Normalise(length);
  }

  ///@brief Calculates the cross product of this vector with @a rhs.
  Vector3  Cross(Vector3 const& rhs) const
  {
    return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y -
      y * rhs.x);
  }

  ///@brief Calculates the reflection of this vector off the given @a normal.
  Vector3 Reflect(Vector3 const& normal) const
  {
    return *this - normal * (2.f * Dot(normal));
  }

  ///@brief Linearly interpolates between this vector and @a to, at the given
  /// blend factor @a t.
  Vector3 Lerp(Vector3 const& to, float t) const
  {
    return Vector3(x + (to.x - x) * t, y + (to.y - y) * t, z + (to.z - z) * t);
  }

  ///@brief Provides a Vector2 using the x and y components of this vector.
  ///@note This method can be used to pack a normal into a Vector2. I.e. if
  /// this is unit length then UnpackNormal(this.XY()) == this.
  Vector2 XY() const
  {
    return Vector2(x, y);
  }

  ///@brief Provides a Vector2 using the y and z components of this vector.
  Vector2 YZ() const
  {
    return Vector2(y, z);
  }

  ///@brief Provides a Vector2 using the z and x components of this vector.
  Vector2 ZX() const
  {
    return Vector2(z, x);
  }

  ///@brief Applies the function @a fn, to pairs of @a value (which is updated
  /// from each execution of @a fn), and each component of this, in this order.
  ///@return The result of performing @a fn with each component and @a value.
  float ComponentWise(float(*fn)(float, float), float value) const
  {
    for (auto f : *this)
    {
      value = fn(value, f);
    }
    return value;
  }

  ///@brief Applies the function @a fn to each component of this, reassigning
  /// the result to the component.
  void ComponentWise(float(*fn)(float))
  {
    for (auto& f : *this)
    {
      f = fn(f);
    }
  }

  // range based for support
  float* begin()
  {
    return &x;
  }

  float* end()
  {
    return &x + kNumComponents;
  }

  float const* begin() const
  {
    return &x;
  }

  float const* end() const
  {
    return &x + kNumComponents;
  }

  // operators
  Vector3& operator +=(Vector3 const& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }

  Vector3  operator +(Vector3 const& rhs) const
  {
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
  }

  Vector3& operator -=(Vector3 const& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }

  Vector3  operator -(Vector3 const& rhs) const
  {
    return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
  }

  Vector3 operator -() const
  {
    return Vector3(-x, -y, -z);
  }

  Vector3&  operator *=(float s)
  {
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }

  Vector3&  operator /=(float s)
  {
    XR_ASSERT(Vector3, s != 0.f);
    s = 1.f / s;
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }

  Vector3  operator *(float s) const
  {
    return Vector3(x * s, y * s, z * s);
  }

  Vector3  operator /(float s) const
  {
    XR_ASSERT(Vector3, s != 0.f);
    s = 1.f / s;
    return Vector3(x * s, y * s, z * s);
  }

  Vector3&  operator*=(Vector3 const& rhs)
  {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
  }

  Vector3 operator*(Vector3 const& rhs) const
  {
    Vector3 temp(*this);
    temp *= rhs;
    return temp;
  }

  Vector3&  operator/=(Vector3 const& rhs)
  {
    XR_ASSERT(Vector3, rhs.x != 0.f);
    x /= rhs.x;
    XR_ASSERT(Vector3, rhs.y != 0.f);
    y /= rhs.y;
    XR_ASSERT(Vector3, rhs.z != 0.f);
    z /= rhs.z;
    return *this;
  }

  Vector3 operator/(Vector3 const& rhs) const
  {
    Vector3 temp(*this);
    temp /= rhs;
    return temp;
  }
};

inline
Vector3 operator *(float lhs, Vector3 const& rhs)
{
  return rhs * lhs;
}

inline
Vector3 operator /(float lhs, Vector3 const& rhs)
{
  return Vector3(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
}

}

#define XR_TRACE_VECTOR3(chnl, vector)\
{\
  xr::Vector3 const& v = (vector);\
  XR_TRACE(chnl, ("%s { %.3f, %.3f, %.3f }", #vector, v.x, v.y, v.z));\
}\

#endif //XR_VECTOR3_HPP

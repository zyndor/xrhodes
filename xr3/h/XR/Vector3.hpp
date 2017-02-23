//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_VECTOR3_HPP
#define XR_VECTOR3_HPP

#include "Vector2.hpp"
#include "debug.hpp"
#include <cstdint>
#include <cmath>

namespace XR
{

//==============================================================================
struct Vector3 
{
  // static
  static Vector3  Zero()
  {
    return Vector3(.0f, .0f, .0f);
  }

  static Vector3  One()
  {
    return Vector3(1.0f, 1.0f, 1.0f);
  }

  static Vector3  UnitX()
  {
    return Vector3(1.0f, .0f, .0f);
  }

  static Vector3  UnitY()
  {
    return Vector3(.0f, 1.0f, .0f);
  }

  static Vector3  UnitZ()
  {
    return Vector3(.0f, .0f, 1.0f);
  }

  // data
  union
  {
    struct 
    {
      float  x, y, z;
    };
    float  arData[3];
  };

  // structors
  Vector3()
  : x(.0f), y(.0f), z(.0f)
  {}
  
  explicit Vector3(const float arData[3])
  : x(arData[0]),
    y(arData[1]),
    z(arData[2])
  {}
  
  Vector3(float x_, float y_, float z_)
  : x(x_),
    y(y_),
    z(z_)
  {}

  // general
  ///@brief Calculates the magnitude of this vector.
  float Magnitude() const
  {
    return sqrtf(Dot());
  }

  ///@brief Calculates the dot product of this vector with itself.
  float Dot() const
  {
    return Dot(*this);
  }

  ///@brief Calculates the dot product of this vector with @a rhs.
  float Dot(const Vector3& rhs) const
  {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }

  ///@brief Normalises this vector.
  Vector3&  Normalise(float s = 1.0f)
  {
    float d(Dot());
    XR_ASSERT(Vector3, d > .0f);
    return this->operator*=(s / sqrtf(d));
  }

  ///@brief Calculates the cross product of this vector with @a rhs.
  Vector3  Cross(const Vector3& rhs) const
  {
    return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y -
      y * rhs.x);
  }

  ///@brief Linearly interpolates between this vector and @a to, at the given @a t blend factor.
  Vector3 Lerp(const Vector3& to, float t) const
  {
    return Vector3(x + (to.x - x) * t, y + (to.y - y) * t, z + (to.z - z) * t);
  }

  ///@brief Provides a Vector2 using the x and y components of this vector.
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

  // operators
  Vector3& operator +=(const Vector3& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }

  Vector3  operator +(const Vector3& rhs) const
  {
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
  }

  Vector3& operator -=(const Vector3& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }

  Vector3  operator -(const Vector3& rhs) const
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
    XR_ASSERT(Vector3, s != .0f);
    s = 1.0f / s;
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
    XR_ASSERT(Vector3, s != .0f);
    s = 1.0f / s;
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
    XR_ASSERT(Vector3, rhs.x != .0f);
    x /= rhs.x;
    XR_ASSERT(Vector3, rhs.y != .0f);
    y /= rhs.y;
    XR_ASSERT(Vector3, rhs.z != .0f);
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

}

#define XR_TRACE_VECTOR3(chnl, vector)\
{\
  XR::Vector3 const& v = (vector);\
  XR_TRACE(chnl, ("%s { %.3f, %.3f, %.3f }", #vector, v.x, v.y, v.z));\
}\

#endif //XR_VECTOR3_HPP
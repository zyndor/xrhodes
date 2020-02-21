#ifndef XR_VECTOR4_HPP
#define XR_VECTOR4_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Quaternion.hpp"
#include "xr/math/Vector3.hpp"
#include "xr/math/Vector2.hpp"

namespace xr
{

//==============================================================================
///@brief A container class for vector types of 4 (or less) components, with
/// functionality to convert between them as well as common vector functionality
/// and arithmetics.
class Vector4
{
public:
  // types
  enum
  {
    X,
    Y,
    Z,
    W,
    kNumComponents
  };

  // static
  ///@return Vector4 packed from up to two Vector2s.
  static Vector4 From(Vector2 const& v, Vector2 const& v2 = Vector2::Zero())
  {
    return Vector4(v.x, v.y, v2.x, v2.y);
  }

  ///@return Vector4 packed from a Vector3 and an optional scalar.
  static Vector4 From(Vector3 const& v, float s = 0.f)
  {
    return Vector4(v.x, v.y, v.z, s);
  }

  ///@return Vector4 packed from a scalar and an optional Vector3.
  static Vector4 From(float s, Vector3 const& v = Vector3::Zero())
  {
    return Vector4(s, v.x, v.y, v.z);
  }

  ///@return Quaternion as a Vector4.
  static Vector4 From(Quaternion const& q)
  {
    return Vector4(q.i, q.j, q.k, q.w);
  }

  // data
  union
  {
    struct
    {
      float  x, y, z, w;
    };
    float  data[kNumComponents];
    Vector2 vec2;
    Vector2 vec22[2];
    Vector3 vec3;
    Quaternion quaternion;
  };

  // structors
  Vector4(float x_ = 0.f, float y_ = 0.f, float z_ = 0.f, float w_ = 0.f)
  : x{ x_ },
    y{ y_ },
    z{ z_ },
    w{ w_ }
  {}

  Vector4(float const* data_, size_t numElements = 4)
  {
    if (numElements > kNumComponents)
    {
      numElements = kNumComponents;
    }

    for (size_t i = 0; i < numElements; ++i)
    {
      data[i] = data_[i];
    }
  }

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
  float Dot(Vector4 const& v) const
  {
    return x * v.x + y * v.y + z * v.z + w * v.w;
  }

  ///@brief Scales the vector so that its length is @a length.
  ///@note This vector must be non-zero.
  Vector4& Normalise(float length = 1.f)
  {
    float d = Dot();
    XR_ASSERT(Vector4, d > 0.f);
    return this->operator*=(length / std::sqrt(d));
  }

  //@return A copy of this vector, normalised to @a length.
  ///@note This vector must be non-zero.
  Vector4 Normalised(float length = 1.f) const
  {
    Vector4 result(*this);
    return result.Normalise(length);
  }

  // operators
  Vector4& operator+=(Vector4 const& v)
  {
    auto readp = v.data;
    for (auto& i: data)
    {
      i += *readp;
      ++readp;
    }
    return *this;
  }

  Vector4 operator+(Vector4 const& v) const
  {
    Vector4 tmp(*this);
    return tmp += v;
  }

  Vector4& operator-=(Vector4 const& v)
  {
    auto readp = v.data;
    for (auto& i : data)
    {
      i -= *readp;
      ++readp;
    }
    return *this;
  }

  Vector4 operator-(Vector4 const& v) const
  {
    Vector4 tmp(*this);
    return tmp -= v;
  }

  Vector4& operator*=(float s)
  {
    for (auto& i : data)
    {
      i *= s;
    }
    return *this;
  }

  Vector4 operator*(float s) const
  {
    Vector4 tmp(*this);
    return tmp *= s;
  }

  Vector4& operator/=(float s)
  {
    XR_ASSERT(Vector4, s * s > 0.f);
    s = 1.f / s;
    for (auto& i : data)
    {
      i *= s;
    }
    return *this;
  }

  Vector4 operator/(float s) const
  {
    Vector4 tmp(*this);
    return tmp /= s;
  }

  Vector4 operator-() const
  {
    return Vector4(-x, -y, -z, -w);
  }
};

}

#endif //XR_VECTOR4_HPP

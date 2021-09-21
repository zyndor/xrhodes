#ifndef XR_MATRIX_HPP
#define XR_MATRIX_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/math/Vector3.hpp"
#include "mathutils.hpp"
#include "xr/debug.hpp"
#include <algorithm>
#include <cstring>
#include <cstdint>
#include <cmath>

namespace xr
{

//==============================================================================
///@brief Row-major order matrix with a 3x3 linear transformation (rotation,
/// scaling) and 1x3 translation components.
class Matrix
{
public:
  // types
  enum
  {
    // components of x axis - right
    XX,
    XY,
    XZ,
    // components of y axis - up
    YX,
    YY,
    YZ,
    // components of z axis - forward
    ZX,
    ZY,
    ZZ,
    kNumLinearComponents
  };

  // static
  inline
  constexpr static Matrix Identity()
  {
    return Matrix();
  }

  inline
  static void Transpose(float data[kNumLinearComponents])
  {
    std::swap(data[XY], data[YX]);
    std::swap(data[ZX], data[XZ]);
    std::swap(data[YZ], data[ZY]);
  }

  // data
  float  linear[kNumLinearComponents];
  Vector3 t;

  // structors
  constexpr explicit Matrix(Vector3 const& t_ = Vector3::Zero())
  : linear{ 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f },
    t{ t_ }
  {}

  constexpr explicit Matrix(const float linearXform[kNumLinearComponents],
    Vector3 const& t_ = Vector3::Zero())
  : linear{ linearXform[XX], linearXform[XY], linearXform[XZ],
      linearXform[YX], linearXform[YY], linearXform[YZ],
      linearXform[ZX], linearXform[ZY], linearXform[ZZ] },
    t{ t_ }
  {}

  constexpr Matrix(Matrix const& rhs, Vector3 const& t_)
  : Matrix(rhs.linear, t_)
  {}

  // general
  constexpr float& xx() { return linear[XX]; }
  constexpr float xx() const { return linear[XX]; }

  constexpr float& xy() { return linear[XY]; }
  constexpr float xy() const { return linear[XY]; }

  constexpr float& xz() { return linear[XZ]; }
  constexpr float xz() const { return linear[XZ]; }

  constexpr float& yx() { return linear[YX]; }
  constexpr float yx() const { return linear[YX]; }

  constexpr float& yy() { return linear[YY]; }
  constexpr float yy() const { return linear[YY]; }

  constexpr float& yz() { return linear[YZ]; }
  constexpr float yz() const { return linear[YZ]; }

  constexpr float& zx() { return linear[ZX]; }
  constexpr float zx() const { return linear[ZX]; }

  constexpr float& zy() { return linear[ZY]; }
  constexpr float zy() const { return linear[ZY]; }

  constexpr float& zz() { return linear[ZZ]; }
  constexpr float zz() const { return linear[ZZ]; }

  ///@return Column @a i of the linear transformation part.
  Vector3 GetColumn(size_t i) const
  {
    XR_ASSERT(Matrix, i < Vector3::kNumComponents);
    return Vector3(linear[i], linear[i + Vector3::kNumComponents],
      linear[i + 2 * Vector3::kNumComponents]);
  }

  ///@brief Convenience method to set column @a i of the linear transformation part.
  void SetColumn(size_t i, Vector3 const& v)
  {
    XR_ASSERT(Matrix, i < Vector3::kNumComponents);
    linear[i] = v.x;
    linear[i + Vector3::kNumComponents] = v.y;
    linear[i + 2 * Vector3::kNumComponents] = v.z;
  }

  ///@brief Sets the linear transformation component of the Matrix to be an
  /// an elemental rotation of @a theta radians counter-clockwise, around the
  /// positive X axis. @a resetLinear determines whether the elements of the
  /// matrix not involved in creating the rotation shall be set to 0.
  void SetRotationX(float theta, bool resetLinear)
  {
    linear[XX] = 1.f;
    const float c = std::cos(theta);
    linear[YY] = c;
    linear[ZZ] = c;
    const float s = std::sin(theta);
    linear[ZY] = s;
    linear[YZ] = -s;

    if (resetLinear)
    {
      linear[XY] = 0.f;
      linear[XZ] = 0.f;
      linear[YX] = 0.f;
      linear[ZX] = 0.f;
    }
  }

  ///@brief Sets the linear transformation component of the Matrix to be an
  /// an elemental rotation of @a theta radians counter-clockwise, around the
  /// positive Y axis. @a resetLinear determines whether the elements of the
  /// matrix not involved in creating the rotation shall be set to 0.
  void SetRotationY(float theta, bool resetLinear)
  {
    linear[YY] = 1.f;
    const float c = std::cos(theta);
    linear[ZZ] = c;
    linear[XX] = c;
    const float s = std::sin(theta);
    linear[XZ] = s;
    linear[ZX] = -s;

    if (resetLinear)
    {
      linear[XY] = 0.f;
      linear[YX] = 0.f;
      linear[YZ] = 0.f;
      linear[ZY] = 0.f;
    }
  }

  ///@brief Sets the linear transformation component of the Matrix to be an
  /// an elemental rotation of @a theta radians counter-clockwise, around the
  /// positive Z axis. @a resetLinear determines whether the elements of the
  /// matrix not involved in creating the rotation shall be set to 0.
  void SetRotationZ(float theta, bool resetLinear)
  {
    linear[ZZ] = 1.f;
    const float c = std::cos(theta);
    linear[XX] = c;
    linear[YY] = c;
    const float s = std::sin(theta);
    linear[YX] = s;
    linear[XY] = -s;

    if (resetLinear)
    {
      linear[XZ] = 0.f;
      linear[YZ] = 0.f;
      linear[ZX] = 0.f;
      linear[ZY] = 0.f;
    }
  }

  ///@brief Scales the x component of the linear transformation by the scalar @a s.
  constexpr void  ScaleX(float s)
  {
    linear[XX] *= s;
    linear[XY] *= s;
    linear[XZ] *= s;
  }

  ///@brief Scales the y component of the linear transformation by the scalar @a s.
  constexpr void  ScaleY(float s)
  {
    linear[YX] *= s;
    linear[YY] *= s;
    linear[YZ] *= s;
  }

  ///@brief Scales the z component of the linear transformation by the scalar @a s.
  constexpr void  ScaleZ(float s)
  {
    linear[ZX] *= s;
    linear[ZY] *= s;
    linear[ZZ] *= s;
  }

  ///@brief Scales the linear transformation part of this Matrix by the scalar @a s.
  constexpr void  ScaleLinear(float s)
  {
    linear[XX] *= s;
    linear[XY] *= s;
    linear[XZ] *= s;
    linear[YX] *= s;
    linear[YY] *= s;
    linear[YZ] *= s;
    linear[ZX] *= s;
    linear[ZY] *= s;
    linear[ZZ] *= s;
  }

  ///@return The scaling along the X axis.
  float GetXScaling() const
  {
    return Vector3(linear + XX).Magnitude();
  }

  ///@return The scaling along the Y axis.
  float GetYScaling() const
  {
    return Vector3(linear + YX).Magnitude();
  }

  ///@return The scaling along the Z axis.
  float GetZScaling() const
  {
    return Vector3(linear + ZX).Magnitude();
  }

  ///@brief Copies the linear transformation part from the given array.
  void  CopyLinear(const float source[kNumLinearComponents])
  {
    std::copy(source, source + kNumLinearComponents, linear);
  }

  ///@brief Copies the linear transformation part of the given matrix.
  void  CopyLinear(Matrix const& m)
  {
    CopyLinear(m.linear);
  }

  ///@brief Multiplies the linear transformation component of this Matrix
  /// by the linear transformation component the other Matrix @a m.
  /// No translation applied or modified.
  void  RotateBy(Matrix const& m)
  {
    Matrix  n(*this);
    CalculateComponentProduct(m, n, Vector3::X, Vector3::X);
    CalculateComponentProduct(m, n, Vector3::X, Vector3::Y);
    CalculateComponentProduct(m, n, Vector3::X, Vector3::Z);
    CalculateComponentProduct(m, n, Vector3::Y, Vector3::X);
    CalculateComponentProduct(m, n, Vector3::Y, Vector3::Y);
    CalculateComponentProduct(m, n, Vector3::Y, Vector3::Z);
    CalculateComponentProduct(m, n, Vector3::Z, Vector3::X);
    CalculateComponentProduct(m, n, Vector3::Z, Vector3::Y);
    CalculateComponentProduct(m, n, Vector3::Z, Vector3::Z);
  }

  void  CalculateComponentProduct(Matrix const& m, Matrix const& n, int i, int j)
  {
    XR_ASSERT(Matrix, i >= 0);
    XR_ASSERT(Matrix, i < Vector3::kNumComponents);
    XR_ASSERT(Matrix, j >= 0);
    XR_ASSERT(Matrix, j < Vector3::kNumComponents);

    int y(j * Vector3::kNumComponents);
    linear[y + i] = m.linear[y] * n.linear[i] +
      m.linear[y + 1] * n.linear[i + Vector3::kNumComponents] +
      m.linear[y + 2] * n.linear[i + Vector3::kNumComponents * 2];
  }

  ///@brief Transforms this matrix by the matrix @a m, applying its
  /// rotation and translation.
  void  TransformBy(Matrix const& m)
  {
    RotateBy(m);
    t = m.Transform(t);
  }

  ///@brief Rotates the vector by this matrix.
  Vector3  Rotate(Vector3 const& v) const
  {
    return Vector3(v.x * linear[XX] + v.y * linear[YX] + v.z * linear[ZX],
      v.x * linear[XY] + v.y * linear[YY] + v.z * linear[ZY],
      v.x * linear[XZ] + v.y * linear[YZ] + v.z * linear[ZZ]);
  }

  [[deprecated("Use Rotate().")]]
  Vector3 RotateVec(Vector3 const& v) const
  {
    return Rotate(v);
  }

  ///@brief Transforms the vector by this matrix.
  Vector3  Transform(Vector3 const& v) const
  {
    return Rotate(v) + t;
  }

  [[deprecated("Use Transform().")]]
  Vector3 TransformVec(Vector3 const& v) const
  {
    return Transform(v);
  }

  ///@brief Calculates a transformation looking from the position @a from to
  /// @a to, with the given @a up vector.
  void  LookAt(Vector3 const& from, Vector3 const& to, Vector3 const& up)
  {
    Vector3  vz(to - from);
    float dz = vz.Dot(vz);
    if (dz > .0f)
    {
      dz = 1.0f / std::sqrt(dz);
      vz *= dz;
    }
    else
    {
      vz.x = 1.0f;
    }

    Vector3  vx(up.Cross(vz));  // local x axis
    vx.Normalise();
    Vector3  vy(vz.Cross(vx));  // local y axis
    vy.Normalise();
    std::copy(vx.begin(), vx.end(), linear + XX);
    std::copy(vy.begin(), vy.end(), linear + YX);
    std::copy(vz.begin(), vz.end(), linear + ZX);
  }

  ///@brief Calculates a transformation looking from the translation part
  /// of the matrix used as a position, to @a to, with the given @a up vector.
  void  LookAt(Vector3 const& to, Vector3 const& up)
  {
    LookAt(Vector3(t.x, t.y, t.z), to, up);
  }

  ///@brief Attempts to calculate the inverse of the linear transformation part
  /// and if succeeded, replaces it with the inverse.
  bool  Invert()
  {
    // Calculate minors and cofactors 2 in 1.
    decltype(linear) adj;
    adj[XX] = linear[YY] * linear[ZZ] - linear[ZY] * linear[YZ];
    adj[XY] = linear[ZX] * linear[YZ] - linear[YX] * linear[ZZ]; // negated
    adj[XZ] = linear[YX] * linear[ZY] - linear[YY] * linear[ZX];

    adj[YX] = linear[ZY] * linear[XZ] - linear[XY] * linear[ZZ]; // negated
    adj[YY] = linear[XX] * linear[ZZ] - linear[XZ] * linear[ZX];
    adj[YZ] = linear[XY] * linear[ZX] - linear[XX] * linear[ZY]; // negated

    adj[ZX] = linear[XY] * linear[YZ] - linear[XZ] * linear[YY];
    adj[ZY] = linear[XZ] * linear[YX] - linear[XX] * linear[YZ]; // negated
    adj[ZZ] = linear[XX] * linear[YY] - linear[XY] * linear[YX];

    float determinant = linear[XX] * adj[XX] + linear[XY] * adj[XY] +
      linear[XZ] * adj[XZ];
    const bool result = determinant * determinant > kEpsilon;
    if (result)
    {
      // Transpose to get adjugate matrix.
      Transpose(adj);

      determinant = 1.0f / determinant;

      // Scale by inverse detereminant and write back.
      auto writep = linear;
      std::for_each(adj, adj + std::extent<decltype(adj)>::value,
        [determinant, &writep](float v) {
          *writep = v * determinant;
          ++writep;
        });
    }
    return result;
  }

  ///@brief Changes the linear transformation part of this matrix to its transpose.
  Matrix&  Transpose()
  {
    Transpose(linear);
    return *this;
  }

  ///@brief Calculates the transpose of the linear transformation part of this matrix.
  Matrix  Transposed() const
  {
    return Matrix(*this).Transpose();
  }

  // operator overloads
  Matrix&  operator*=(Matrix const& rhs)
  {
    TransformBy(rhs);
    return *this;
  }

  Matrix  operator*(Matrix const& rhs) const
  {
    Matrix  product(*this);
    return product *= rhs;
  }
};

}

#define XR_TRACE_MATRIX(chnl, matrix)\
{\
  xr::Matrix const& m = (matrix);\
  XR_TRACE(chnl, ("%s { %.3f, %.3f, %.3f\n\t%.3f, %.3f, %.3f\n\t%.3f, %.3f, %.3f } }",\
    #matrix, m.xx(), m.xy(), m.xz(), m.yx(), m.yy(), m.yz(), m.zx(), m.zy(), m.zz()));\
}\


#endif //XR_MATRIX_HPP

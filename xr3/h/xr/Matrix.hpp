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
#include "Vector3.hpp"
#include "maths.hpp"
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
  static Matrix Identity()
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
  union
  {
    // As individual components
    struct
    {
      float  xx, xy, xz, // local x axis
              yx, yy, yz, // local y axis
              zx, zy, zz; // local z axis
    };
    // Linear transformation component as a 1D array
    float  linear[kNumLinearComponents];
    // Linear transformation component as a 2D array
    float  linear2d[Vector3::kNumComponents][Vector3::kNumComponents];
  };
  Vector3 t;

  // structors
  explicit Matrix(Vector3 const& t_ = Vector3::Zero())
  : xx(1.0f), xy(.0f), xz(.0f),
    yx(.0f), yy(1.0f), yz(.0f),
    zx(.0f), zy(.0f), zz(1.0f),
    t(t_)
  {}

  explicit Matrix(const float linearXform[kNumLinearComponents],
    Vector3 const& t_ = Vector3::Zero())
  : xx(linearXform[XX]), xy(linearXform[XY]), xz(linearXform[XZ]),
    yx(linearXform[YX]), yy(linearXform[YY]), yz(linearXform[YZ]),
    zx(linearXform[ZX]), zy(linearXform[ZY]), zz(linearXform[ZZ]),
    t(t_)
  {}

  Matrix(Matrix const& rhs, Vector3 const& t_)
  : Matrix(rhs.linear, t_)
  {}

  // general
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
    xx = 1.f;
    const float c = std::cos(theta);
    yy = c;
    zz = c;
    const float s = std::sin(theta);
    zy = s;
    yz = -s;

    if (resetLinear)
    {
      xy = 0.f;
      xz = 0.f;
      yx = 0.f;
      zx = 0.f;
    }
  }

  ///@brief Sets the linear transformation component of the Matrix to be an
  /// an elemental rotation of @a theta radians counter-clockwise, around the
  /// positive Y axis. @a resetLinear determines whether the elements of the
  /// matrix not involved in creating the rotation shall be set to 0.
  void SetRotationY(float theta, bool resetLinear)
  {
    yy = 1.f;
    const float c = std::cos(theta);
    zz = c;
    xx = c;
    const float s = std::sin(theta);
    zx = -s;
    xz = s;

    if (resetLinear)
    {
      xy = 0.f;
      yx = 0.f;
      yz = 0.f;
      zy = 0.f;
    }
  }

  ///@brief Sets the linear transformation component of the Matrix to be an
  /// an elemental rotation of @a theta radians counter-clockwise, around the
  /// positive Z axis. @a resetLinear determines whether the elements of the
  /// matrix not involved in creating the rotation shall be set to 0.
  void SetRotationZ(float theta, bool resetLinear)
  {
    zz = 1.f;
    const float c = std::cos(theta);
    xx = c;
    yy = c;
    const float s = std::sin(theta);
    xy = -s;
    yx = s;

    if (resetLinear)
    {
      xz = 0.f;
      yz = 0.f;
      zx = 0.f;
      zy = 0.f;
    }
  }

  ///@brief Scales the x component of the linear transformation by the scalar @a s.
  void  ScaleX(float s)
  {
    xx *= s;
    xy *= s;
    xz *= s;
  }

  ///@brief Scales the y component of the linear transformation by the scalar @a s.
  void  ScaleY(float s)
  {
    yx *= s;
    yy *= s;
    yz *= s;
  }

  ///@brief Scales the z component of the linear transformation by the scalar @a s.
  void  ScaleZ(float s)
  {
    zx *= s;
    zy *= s;
    zz *= s;
  }

  ///@brief Scales the linear transformation part of this Matrix by the scalar @a s.
  void  ScaleLinear(float s)
  {
    xx *= s;
    xy *= s;
    xz *= s;
    yx *= s;
    yy *= s;
    yz *= s;
    zx *= s;
    zy *= s;
    zz *= s;
  }

  ///@brief Copies the linear transformation part from the given array.
  void  CopyLinear(const float source[kNumLinearComponents])
  {
    memcpy(linear, source, sizeof(linear));
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
    t = m.TransformVec(t);
  }

  ///@brief Rotates the vector by this matrix.
  Vector3  RotateVec(Vector3 const& v) const
  {
    return Vector3(v.x * xx + v.y * yx + v.z * zx,
      v.x * xy + v.y * yy + v.z * zy,
      v.x * xz + v.y * yz + v.z * zz);
  }

  ///@brief Transforms the vector by this matrix.
  Vector3  TransformVec(Vector3 const& v) const
  {
    return RotateVec(v) + t;
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
    memcpy(linear + XX, vx.data, sizeof(vx));
    memcpy(linear + YX, vy.data, sizeof(vy));
    memcpy(linear + ZX, vz.data, sizeof(vz));
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
    #matrix, m.xx, m.xy, m.xz, m.yx, m.yy, m.yz, m.zx, m.zy, m.zz));\
}\


#endif //XR_MATRIX_HPP

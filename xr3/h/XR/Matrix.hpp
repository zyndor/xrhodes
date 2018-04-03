#ifndef XR_MATRIX_HPP
#define XR_MATRIX_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "Vector3.hpp"
#include "maths.hpp"
#include "XR/debug.hpp"
#include <algorithm>
#include <cstring>
#include <cstdint>
#include <cmath>

namespace XR
{

//==============================================================================
///@brief Row-major order matrix with a 3x3 linear transformation (rotation,
/// scaling) and 1x3 translation components.
struct Matrix
{
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
  static Matrix Identity()
  {
    return Matrix();
  }

  // data
  union
  {
    // As a 1D array
    float  arLinear[kNumLinearComponents];
    // As a 2D array
    float  arLinear2D[Vector3::kNumComponents][Vector3::kNumComponents];
    // As individual components
    struct
    {
      float  xx, xy, xz, // local x axis
              yx, yy, yz, // local y axis
              zx, zy, zz; // local z axis
    };
  };
  Vector3 t;

  // structors
  Matrix()
  : Matrix(Vector3::Zero())
  {}

  explicit Matrix(Vector3 const& t_)
  : xx(1.0f), xy(.0f), xz(.0f),
    yx(.0f), yy(1.0f), yz(.0f),
    zx(.0f), zy(.0f), zz(1.0f),
    t(t_)
  {}

  explicit Matrix(const float arData[kNumLinearComponents],
    Vector3 const& t_ = Vector3::Zero())
  : xx(arData[XX]), xy(arData[XY]), xz(arData[XZ]),
    yx(arData[YX]), yy(arData[YY]), yz(arData[YZ]),
    zx(arData[ZX]), zy(arData[ZY]), zz(arData[ZZ]),
    t(t_)
  {}

  Matrix(Matrix const& rhs, Vector3 const& t_)
  : Matrix(rhs.arLinear, t_)
  {}

  // general
  Vector3 GetColumn(size_t i) const
  {
    XR_ASSERT(Matrix, i < Vector3::kNumComponents);
    return Vector3(arLinear[i], arLinear[i + Vector3::kNumComponents],
      arLinear[i + 2 * Vector3::kNumComponents]);
  }

  void SetColumn(size_t i, Vector3 const& v)
  {
    XR_ASSERT(Matrix, i < Vector3::kNumComponents);
    arLinear[i] = v.x;
    arLinear[i + Vector3::kNumComponents] = v.y;
    arLinear[i + 2 * Vector3::kNumComponents] = v.z;
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
  void  ScaleRot(float s)
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

  ///@brief Sets the matrix to represent a rotation around the X axis,
  /// with the option to reset the translation part (@a resetTrans)
  /// and set the value of the components that aren't involved in the
  /// rotation, to zero (@a setZeros).
  void  SetRotX(float theta, bool resetTrans, bool setZeros)
  {
    if (resetTrans)
    {
      t = Vector3::Zero();
    }

    if (setZeros)
    {
      xy = .0f;
      xz = .0f;
      yx = .0f;
      zx = .0f;
    }

    xx = 1.0f;
    yy = zz = cosf(theta);
    yz = -(zy = sinf(theta));
  }

  ///@brief Sets the matrix to represent a rotation around the Y axis,
  /// with the option to reset the translation part (@a resetTrans)
  /// and set the value of the components that aren't involved in the
  /// rotation, to zero (@a setZeros).
  void  SetRotY(float theta, bool resetTrans, bool setZeros)
  {
    if (resetTrans)
    {
      t = Vector3::Zero();
    }

    if (setZeros)
    {
      xy = .0f;
      yz = .0f;
      yx = .0f;
      zy = .0f;
    }

    yy = 1.0f;
    xx = zz = cosf(theta);
    zx = -(xz = sinf(theta));
  }

  ///@brief Sets the matrix to represent a rotation around the Z axis,
  /// with the option to reset the translation part (@a resetTrans)
  /// and set the value of the components that aren't involved in the
  /// rotation, to zero (@a setZeros).
  void  SetRotZ(float theta, bool resetTrans, bool setZeros)
  {
    if (resetTrans)
    {
      t = Vector3::Zero();
    }

    if (setZeros)
    {
      xz = .0f;
      yz = .0f;
      zx = .0f;
      zy = .0f;
    }

    zz = 1.0f;
    xx = yy = cosf(theta);
    xy = -(yx = sinf(theta));
  }

  ///@brief Transform the linear transformation part of the matrix only, by
  /// rotating it @a theta degrees around the X axis.
  void  RotateX(float theta)
  {
    Matrix  m;
    m.SetRotX(theta, true, true);
    RotateBy(m);
  }

  ///@brief Transform the linear transformation part of the matrix only, by
  /// rotating it @a theta degrees around the Y axis.
  void  RotateY(float theta)
  {
    Matrix  m;
    m.SetRotY(theta, true, true);
    RotateBy(m);
  }

  ///@brief Transform the linear transformation part of the matrix only, by
  /// rotating it @a theta degrees around the Z axis.
  void  RotateZ(float theta)
  {
    Matrix  m;
    m.SetRotZ(theta, true, true);
    RotateBy(m);
  }

  ///@brief Calculates the amount of rotation the matrix represents around the
  /// X axis.
  float DirX() const
  {
    Vector3 v(RotateVec(Vector3::UnitX()));
    return  atan2f(v.y, v.z);
  }

  ///@brief Calculates the amount of rotation the matrix represents around the
  /// Y axis.
  float DirY() const
  {
    Vector3 v(RotateVec(Vector3::UnitY()));
    return  atan2f(v.x, v.z);
  }

  ///@brief Calculates the amount of rotation the matrix represents around the
  /// Z axis.
  float DirZ() const
  {
    Vector3 v(RotateVec(Vector3::UnitZ()));
    return  atan2f(v.y, v.x);
  }

  ///@brief Copies the linear transformation part from the given array.
  void  CopyLinear(const float parLinear[kNumLinearComponents])
  {
    memcpy(arLinear, parLinear, sizeof(arLinear));
  }

  ///@brief Copies the linear transformation part of the given matrix.
  void  CopyLinear(Matrix const& m)
  {
    CopyLinear(m.arLinear);
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
    arLinear[y + i] = m.arLinear[y] * n.arLinear[i] +
      m.arLinear[y + 1] * n.arLinear[i + Vector3::kNumComponents] +
      m.arLinear[y + 2] * n.arLinear[i + Vector3::kNumComponents * 2];
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
    float dz(vz.Dot(vz));
    if (dz > .0f)
    {
      dz = 1.0f / sqrtf(dz);
      vz *= dz;
    }
    else
    {
      vz.x = 1.0f;
    }

    Vector3  vx(up.Cross(vz));  // local x axis
    Vector3  vy(vz.Cross(vx));  // local y axis
    memcpy(arLinear + XX, vx.arData, sizeof(vx));
    memcpy(arLinear + YX, vy.arData, sizeof(vy));
    memcpy(arLinear + ZX, vz.arData, sizeof(vz));
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
    decltype(arLinear) adj;
    adj[XX] = arLinear[YY] * arLinear[ZZ] - arLinear[ZY] * arLinear[YZ];
    adj[XY] = arLinear[ZX] * arLinear[YZ] - arLinear[YX] * arLinear[ZZ]; // negated
    adj[XZ] = arLinear[YX] * arLinear[ZY] - arLinear[YY] * arLinear[ZX];

    adj[YX] = arLinear[ZY] * arLinear[XZ] - arLinear[XY] * arLinear[ZZ]; // negated
    adj[YY] = arLinear[XX] * arLinear[ZZ] - arLinear[XZ] * arLinear[ZX];
    adj[YZ] = arLinear[XY] * arLinear[ZX] - arLinear[XX] * arLinear[ZY]; // negated

    adj[ZX] = arLinear[XY] * arLinear[YZ] - arLinear[XZ] * arLinear[YY];
    adj[ZY] = arLinear[XZ] * arLinear[YX] - arLinear[XX] * arLinear[YZ]; // negated
    adj[ZZ] = arLinear[XX] * arLinear[YY] - arLinear[XY] * arLinear[YX];

    float determinant = arLinear[XX] * adj[XX] + arLinear[XY] * adj[XY] +
      arLinear[XZ] * adj[XZ];
    const bool result = determinant * determinant > kEpsilon;
    if (result)
    {
      // Transpose to get adjugate matrix.
      std::swap(adj[XY], adj[YX]);
      std::swap(adj[XZ], adj[ZX]);
      std::swap(adj[YZ], adj[ZY]);

      determinant = 1.0f / determinant;

      // Scale by inverse detereminant and write back.
      auto writep = arLinear;
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
    std::swap(arLinear[XY], arLinear[YX]);
    std::swap(arLinear[ZX], arLinear[XZ]);
    std::swap(arLinear[YZ], arLinear[ZY]);
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
  XR::Matrix const& m = (matrix);\
  XR_TRACE(chnl, ("%s { %.3f, %.3f, %.3f\n\t%.3f, %.3f, %.3f\n\t%.3f, %.3f, %.3f } }",\
    #matrix, m.xx, m.xy, m.xz, m.yx, m.yy, m.yz, m.zx, m.zy, m.zz));\
}\


#endif //XR_MATRIX_HPP

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_MATRIX_HPP
#define XR_MATRIX_HPP

#include "Vector3.hpp"
#include "debug.hpp"
#include <algorithm>
#include <cstring>
#include <cstdint>
#include <cmath>

namespace XR
{

//==============================================================================
enum  MatrixInds
{
  // components of x axis - right
  MXX,
  MXY,
  MXZ,
  // components of y axis - up
  MYX,
  MYY,
  MYZ,
  // components of z axis - forward
  MZX,
  MZY,
  MZZ,
  kNumMatrixInds
};

//==============================================================================
///@brief Row-order matrix with a 3x3 linear transformation (rotation, scaling)
/// and 1x3 translation components.
struct Matrix 
{
  // static
  static const float  karIdentityData[kNumMatrixInds];
  
  static Matrix Identity()
  {
    float arData[] = { 1.0f, .0f, .0f, .0f, 1.0f, .0f, .0f, .0f, 1.0f };
    return Matrix(arData);
  }
  
  // data
  union
  {
    // As a 1D array
    float  arLinear[kNumMatrixInds];
    // As a 2D array
    float  arLinear2D[3][3];
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
  {}

  explicit Matrix(const Vector3& t_)
  : t(t_)
  {
    memcpy(arLinear, karIdentityData, sizeof(arLinear));
  }

  explicit Matrix(const float arData[kNumMatrixInds])
  : t()
  {
    CopyLinear(arData);
  }

  Matrix(const Matrix& rhs, const Vector3& t_)
  : t(t_)
  {
    CopyLinear(rhs);
  }

  // general
  Vector3 GetColumn(size_t i) const
  {
    XR_ASSERT(Matrix, i < 3);
    return Vector3(arLinear[i], arLinear[i + 3], arLinear[i + 2 * 3]);
  }

  void SetColumn(size_t i, Vector3 const& v)
  {
    XR_ASSERT(Matrix, i < 3);
    arLinear[i] = v.x;
    arLinear[i + 3] = v.y;
    arLinear[i + 2 * 3] = v.z;
  }

  void  ScaleX(float s)
  {
    xx *= s;
    xy *= s;
    xz *= s;
  }
  
  void  ScaleY(float s)
  {
    yx *= s;
    yy *= s;
    yz *= s;
  }
  
  void  ScaleZ(float s)
  {
    zx *= s;
    zy *= s;
    zz *= s;
  }
  
  ///@brief Scales the rotation part of this matrix by the scalar @a s.
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
  
  ///@brief Transform the rotation part of the matrix only, by
  /// rotating it @a theta degrees around the X axis.
  void  RotateX(float theta)
  {
    Matrix  m;
    m.SetRotX(theta, true, true);
    RotateBy(m);
  }

  ///@brief Transform the rotation part of the matrix only, by
  /// rotating it @a theta degrees around the Y axis.
  void  RotateY(float theta)
  {
    Matrix  m;
    m.SetRotY(theta, true, true);
    RotateBy(m);
  }

  ///@brief Transform the rotation part of the matrix only, by
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
  void  CopyLinear(const float parLinear[kNumMatrixInds])
  {
    memcpy(arLinear, parLinear, sizeof(arLinear));
  }
  
  ///@brief Copies the linear transformation part of the given matrix.
  void  CopyLinear(const Matrix& m)
  {
    CopyLinear(m.arLinear);
  }
  
  ///@brief Multiplies the linear transformation component of this Matrix
  /// by the linear transformation component the other Matrix @a m.
  /// No translation applied or modified.
  void  RotateBy(const Matrix& m)
  {
    Matrix  n(*this);
    CalculateComponentProduct(m, n, 0, 0);
    CalculateComponentProduct(m, n, 0, 1);
    CalculateComponentProduct(m, n, 0, 2);
    CalculateComponentProduct(m, n, 1, 0);
    CalculateComponentProduct(m, n, 1, 1);
    CalculateComponentProduct(m, n, 1, 2);
    CalculateComponentProduct(m, n, 2, 0);
    CalculateComponentProduct(m, n, 2, 1);
    CalculateComponentProduct(m, n, 2, 2);
  }
  
  void  CalculateComponentProduct(const Matrix& m, const Matrix& n, int i, int j)
  {
    XR_ASSERT(Matrix, i >= 0);
    XR_ASSERT(Matrix, i < 3);
    XR_ASSERT(Matrix, j >= 0);
    XR_ASSERT(Matrix, j < 3);

    int y(j * 3);
    arLinear[y + i] = m.arLinear[y] * n.arLinear[i] +
      m.arLinear[y + 1] * n.arLinear[i + 3] +
      m.arLinear[y + 2] * n.arLinear[i + 3 * 2];
  }

  ///@brief Transforms this matrix by the matrix @a m, applying its
  /// rotation and translation.
  void  TransformBy(const Matrix& m)
  {
    RotateBy(m);
    t = m.TransformVec(t);
  }

  ///@brief Rotates the vector by this matrix.
  Vector3  RotateVec(const Vector3& v) const
  {
    return Vector3(v.x * xx + v.y * yx + v.z * zx,
      v.x * xy + v.y * yy + v.z * zy,
      v.x * xz + v.y * yz + v.z * zz);
  }
  
  ///@brief Transforms the vector by this matrix.
  Vector3  TransformVec(const Vector3& v) const
  {
    return RotateVec(v) + t;
  }
  
  ///@brief Calculates a transformation looking from the position @a from to
  /// @a to, with the given @a up vector.
  void  LookAt(const Vector3& from, const Vector3& to, const Vector3& up)
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
    memcpy(arLinear + MXX, vx.arData, sizeof(vx));
    memcpy(arLinear + MYX, vy.arData, sizeof(vy));
    memcpy(arLinear + MZX, vz.arData, sizeof(vz));
  }

  ///@brief Calculates a transformation looking from the translation part
  /// of the matrix used as a position, to @a to, with the given @a up vector.
  void  LookAt(const Vector3& to, const Vector3& up)
  {
    LookAt(Vector3(t.x, t.y, t.z), to, up);
  }

  ///@brief Writes data from the matrix into the given array in 
  /// OpenGL's 4x4 column-order format.
  void  ToGL(float arDataOut[16]) const
  {
    XR_ASSERT(Matrix, arDataOut);
    arDataOut[0] = xx;
    arDataOut[1] = yx;
    arDataOut[2] = zx;
    arDataOut[3] = .0f;
    arDataOut[4] = xy;
    arDataOut[5] = yy;
    arDataOut[6] = zy;
    arDataOut[7] = .0f;
    arDataOut[8] = xz;
    arDataOut[9] = yz;
    arDataOut[10] = zz;
    arDataOut[11] = .0f;
    arDataOut[12] = t.x;
    arDataOut[13] = t.y;
    arDataOut[14] = t.z;
    arDataOut[15] = 1.0f;
  }
  
  ///@brief Changes the linear transformation part of this matrix to its transpose.
  Matrix&  Transpose()
  {
    std::swap(arLinear[MXY], arLinear[MYX]);
    std::swap(arLinear[MZX], arLinear[MXZ]);
    std::swap(arLinear[MYZ], arLinear[MZY]);
    return *this;
  }
  
  ///@brief Calculates the transpose of the rotation part of this matrix.
  Matrix  Transposed() const
  {
    return Matrix(*this).Transpose();
  }

  // operator overloads
  Matrix&  operator*=(const Matrix& rhs)
  {
    TransformBy(rhs);
    return *this;
  }

  Matrix  operator*(const Matrix& rhs) const
  {
    Matrix  product(*this);
    return product *= rhs;
  }
};

}

#define XR_TRACE_MATRIX(name, matrix)  XR_TRACE(name, ("{ %.3f, %.3f, %.3f\n\t%.3f, %.3f, %.3f\n\t%.3f, %.3f, %.3f } }",\
  name, matrix.xx, matrix.xy, matrix.xz, matrix.yx, matrix.yy, matrix.yz, matrix.zx, matrix.zy, matrix.zz))

#endif //XR_MATRIX_HPP
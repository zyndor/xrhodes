#ifndef XR_MATRIX4_HPP
#define XR_MATRIX4_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Matrix.hpp"
#include "xr/math/Vector3.hpp"

namespace xr
{

//==============================================================================
///@brief 4x4 matrix class intended to facilitate the managing of projection
/// transforms.
class Matrix4
{
public:
  // types
  enum { kNumElems = 16 };

  // data
  float data[kNumElems];

  ///@brief Multiplies this and @a m, and stores the result in @a result.
  /// This means post-multiplication (m * this) for column-major matrices and
  /// pre-multiplication (this * m) for row-major matrices.
  ///@note @a result must not alias this.
  void  Transform(Matrix4 const& m, Matrix4& result) const
  {
    for (int i = 0; i < 4; ++i)
    {
      int iTraverse = i * 4;
      for (int j = 0; j < 4; ++j)
      {
        result.data[iTraverse + j] = m.data[j] * data[iTraverse] +
          m.data[j + 4] * data[iTraverse + 1] +
          m.data[j + 8] * data[iTraverse + 2] +
          m.data[j + 12] * data[iTraverse + 3];
      }
    }
  }

  ///@brief Multiplies this and @a m, and overwrites this with the result.
  /// This means post-multiplication (m * this) for column-major matrices and
  /// pre-multiplication (this * m) for row-major matrices.
  void  Transform(Matrix4 const& m)
  {
    Matrix4 temp(*this);
    temp.Transform(m, *this);
  }

  ///@brief Sets @matrix as an identity matrix with the given @a value along
  /// its diagonal.
  void SetIdentity(float value = 1.0f)
  {
    float* writep = data;
    for (int i = 0; i < 3; ++i)
    {
      *writep = value;
      ++writep;

      float* runEnd = writep + 4;
      while(writep != runEnd)
      {
        *writep = .0f;
        ++writep;
      }
    }

    *writep = value;
  }

  ///@brief Sets this as an identity matrix with the given @a value along its
  /// diagonal. Only writes diagonal. Useful when memset() or initialized to zero.
  void SetIdentityDiagonalOnly(float value = 1.0f)
  {
    float* writep = data;
    for (int i = 0; i < 3; ++i)
    {
      *writep = value;
      writep += 5;
    }

    *writep = value;
  }

  ///@brief Changes this matrix to its transpose.
  void Transpose()
  {
    std::swap(data[1], data[4]);
    std::swap(data[2], data[8]);
    std::swap(data[3], data[12]);
    std::swap(data[6], data[9]);
    std::swap(data[7], data[13]);
    std::swap(data[11], data[14]);
  }

  ///@brief Gets the translation part of this matrix, assuming it to be in
  /// column-major order.
  Vector3 GetTranslation() const
  {
    return Vector3(data[12], data[13], data[14]);
  }

  ///@brief Sets the translation part of this matrix, assuming it to be column-major
  /// order.
  void SetTranslation(Vector3 const& t)
  {
    data[12] = t.x;
    data[13] = t.y;
    data[14] = t.z;
  }

  ///@brief Writes data from the matrix @a m into this, in colum-major order.
  ///@note Refer to Matrix's documentation for its ordering.
  void Import(Matrix const& m)
  {
    data[0] = m.xx;
    data[1] = m.xy;
    data[2] = m.xz;
    data[3] = .0f;
    data[4] = m.yx;
    data[5] = m.yy;
    data[6] = m.yz;
    data[7] = .0f;
    data[8] = m.zx;
    data[9] = m.zy;
    data[10] = m.zz;
    data[11] = .0f;
    data[12] = m.t.x;
    data[13] = m.t.y;
    data[14] = m.t.z;
    data[15] = 1.0f;
  }

  ///@brief Copies data from this, assuming it to be column-major order, into
  /// the given Matrix @a m.
  ///@note Refer to Matrix's documentation for its ordering.
  void Export(Matrix& m) const
  {
    m.xx = data[0];
    m.xy = data[1];
    m.xz = data[2];
    m.yx = data[4];
    m.yy = data[5];
    m.yz = data[6];
    m.zx = data[8];
    m.zy = data[9];
    m.zz = data[10];
    m.t.x = data[12];
    m.t.y = data[13];
    m.t.z = data[14];
  }
};

} // XR

#endif //XR_MATRIX4_HPP

#ifndef XR_MATRIX4HELPER_HPP
#define XR_MATRIX4HELPER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Matrix.hpp"
#include "Vector3.hpp"

namespace XR
{

//=============================================================================
class Matrix4Helper
{
public:
  enum { kNumElems = 16 };

  ///@brief Multiplies @a m0 by @a m1 and stores the result in @a result.
  /// This means post-multiplication (M0 * M1) for column-major matrices and
  /// pre-multiplication (M1 * M0) for row-major matrices.
  static void  Multiply(float const m0[kNumElems], float const m1[kNumElems],
    float result[kNumElems])
  {
    for (int i = 0; i < 4; ++i)
    {
      int iColumn = i * 4;
      for (int j = 0; j < 4; ++j)
      {
        result[iColumn + j] = m0[j] * m1[iColumn] +
          m0[j + 4] * m1[iColumn + 1] +
          m0[j + 8] * m1[iColumn + 2] +
          m0[j + 12] * m1[iColumn + 3];
      }
    }
  }

  ///@brief Sets @matrix as an identity matrix with the given @a value along
  /// its diagonal.
  static void SetIdentity(float matrix[kNumElems], float value)
  {
    float* writep = matrix;
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

  ///@brief Sets @matrix as an identity matrix.
  static void SetIdentity(float matrix[kNumElems])
  {
    SetIdentity(matrix, 1.0f);
  }

  ///@brief Sets @matrix as an identity matrix with the given @a value along
  /// its diagonal. Only writes diagonal. Useful when memset() or initialized
  /// to zero.
  static void SetIdentityDiagonalOnly(float matrix[kNumElems], float value)
  {
    float* writep = matrix;
    for (int i = 0; i < 3; ++i)
    {
      *writep = value;
      writep += 5;
    }

    *writep = value;
  }

  ///@brief Sets @matrix as an identity matrix. Only writes diagonal. Useful
  /// when memset() or initialized to zero.
  static void SetIdentityDiagonalOnly(float matrix[kNumElems])
  {
    SetIdentityDiagonalOnly(matrix, 1.0f);
  }

  ///@brief Gets the translation part of a column-major order matrix;
  static Vector3 GetTranslation(float const matrix[kNumElems])
  {
    return Vector3(matrix[12], matrix[13], matrix[14]);
  }

  ///@brief Writes data from the matrix @a m into the given array in
  /// 4x4 column-major order format.
  static void FromMatrix(Matrix const& m, float result[kNumElems])
  {
    XR_ASSERT(Matrix4Helper, result);
    result[0] = m.xx;
    result[1] = m.xy;
    result[2] = m.xz;
    result[3] = .0f;
    result[4] = m.yx;
    result[5] = m.yy;
    result[6] = m.yz;
    result[7] = .0f;
    result[8] = m.zx;
    result[9] = m.zy;
    result[10] = m.zz;
    result[11] = .0f;
    result[12] = m.t.x;
    result[13] = m.t.y;
    result[14] = m.t.z;
    result[15] = 1.0f;
  }

  ///@brief Copies data from 4x4 column-major order matrix @a source into the
  /// given Matrix @a m.
  static void ToMatrix(float const source[kNumElems], Matrix& m)
  {
    m.xx = source[0];
    m.xy = source[1];
    m.xz = source[2];
    m.yx = source[4];
    m.yy = source[5];
    m.yz = source[6];
    m.zx = source[8];
    m.zy = source[9];
    m.zz = source[10];
    m.t.x = source[12];
    m.t.y = source[13];
    m.t.z = source[14];
  }
};

} // XR

#endif //XR_MATRIX4HELPER_HPP

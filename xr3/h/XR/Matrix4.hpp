#ifndef XR_MATRIX4_HPP
#define XR_MATRIX4_HPP
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

//==============================================================================
///@brief 4x4 matrix class intended to facilitate the managing of projection
/// transforms. Inherently free from any notation of colum / row major ordering.
struct Matrix4
{
  // types
  enum { kNumElems = 16 };

  // data
  float data[kNumElems];

  ///@brief Multiplies @a m0 by this and stores the result in @a result.
  /// This means post-multiplication (M0 * this) for column-major matrices and
  /// pre-multiplication (this * M0) for row-major matrices.
  ///@note @a result must not be this.
  void  Multiply(Matrix4 const& m0, Matrix4& result) const
  {
    for (int i = 0; i < 4; ++i)
    {
      int iTraverse = i * 4;
      for (int j = 0; j < 4; ++j)
      {
        result.data[iTraverse + j] = m0.data[j] * data[iTraverse] +
          m0.data[j + 4] * data[iTraverse + 1] +
          m0.data[j + 8] * data[iTraverse + 2] +
          m0.data[j + 12] * data[iTraverse + 3];
      }
    }
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

  ///@brief Calculates the transpose of this matrix.
  void Transpose()
  {
    std::swap(data[1], data[4]);
    std::swap(data[2], data[8]);
    std::swap(data[3], data[12]);
    std::swap(data[6], data[9]);
    std::swap(data[7], data[13]);
    std::swap(data[11], data[14]);
  }

  ///@brief Gets the translation part of this matrix.
  Vector3 GetTranslation() const
  {
    return Vector3(data[12], data[13], data[14]);
  }

  ///@brief Writes data from the matrix @a m into the this.
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

  ///@brief Copies data from this, into the given Matrix @a m.
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

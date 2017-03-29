//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_MAT4X4HELPER_HPP
#define XR_MAT4X4HELPER_HPP

#include "Vector3.hpp"
#include "Matrix.hpp"

namespace XR
{

//=============================================================================
class Mat4x4Helper
{
public:
  static const size_t kNumElems = 16;

  ///@brief Multiplies @a arM0 by @a arM1 and stores the result in @a arResult.
  /// This means post-multiplication (M0 * M1) for column-major matrices and
  /// pre-multiplication (M1 * M0) for row-major matrices.
  /// OpenGL uses column-major matrices.
  static void  Mult(float const (&arM0)[kNumElems], float const (&arM1)[kNumElems],
    float(&arResult)[kNumElems])
  {
    for (int i = 0; i < 4; ++i)
    {
      int iColumn = i * 4;
      for (int j = 0; j < 4; ++j)
      {
        arResult[iColumn + j] = arM0[j] * arM1[iColumn] +
          arM0[j + 4] * arM1[iColumn + 1] +
          arM0[j + 8] * arM1[iColumn + 2] +
          arM0[j + 12] * arM1[iColumn + 3];
      }
    }
  }

  ///@brief Sets @arMat as an identity matrix.
  static void SetIdentity(float(&arMat)[kNumElems], float value)
  {
    float* pWrite = arMat;
    for (int i = 0; i < 3; ++i)
    {
      *pWrite = value;
      ++pWrite;

      float* pEnd = pWrite + 4;
      while(pWrite != pEnd)
      {
        *pWrite = .0f;
        ++pWrite;
      }
    }

    *pWrite = value;
  }

  ///@brief Sets @arMat as an identity matrix.
  static void SetIdentity(float(&arMat)[kNumElems])
  {
    SetIdentity(arMat, 1.0f);
  }

  ///@brief Gets the translation part of a column-major matrix;
  static Vector3 GetTranslation(float const(&arMat)[kNumElems])
  {
    return Vector3(arMat[12], arMat[13], arMat[14]);
  }

  ///@brief Writes data from the matrix @a m into the given array in 
  /// OpenGL's 4x4 column-major format.
  static void ExportToGL(Matrix const& m, float (&arDataOut)[kNumElems])
  {
    XR_ASSERT(Mat4x4Helper, arDataOut);
    arDataOut[0] = m.xx;
    arDataOut[1] = m.yx;
    arDataOut[2] = m.zx;
    arDataOut[3] = .0f;
    arDataOut[4] = m.xy;
    arDataOut[5] = m.yy;
    arDataOut[6] = m.zy;
    arDataOut[7] = .0f;
    arDataOut[8] = m.xz;
    arDataOut[9] = m.yz;
    arDataOut[10] = m.zz;
    arDataOut[11] = .0f;
    arDataOut[12] = m.t.x;
    arDataOut[13] = m.t.y;
    arDataOut[14] = m.t.z;
    arDataOut[15] = 1.0f;
  }
};

} // XR

#endif //XR_MAT4X4HELPER_HPP
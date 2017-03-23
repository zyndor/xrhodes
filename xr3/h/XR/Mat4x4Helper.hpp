//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_MAT4X4HELPER_HPP
#define XR_MAT4X4HELPER_HPP

#include "Vector3.hpp"

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
};

} // XR

#endif //XR_MAT4X4HELPER_HPP
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_AUTOMATRIX_HPP
#define XR_AUTOMATRIX_HPP

#include "Renderer.hpp"
#include "fundamentals.hpp"

namespace XR
{

//==============================================================================
class AutoMatrix: Noncopyable
{
public:
  explicit AutoMatrix(CIwFMat m)
  : matrix(Renderer::GetModelMatrix())
  {
    Renderer::SetModelMatrix(m * matrix);
  }

  ~AutoMatrix()
  {
    Renderer::SetModelMatrix(&matrix);
  }

  Matrix matrix;
};

} // XR

#endif // XR_AUTOMATRIX_HPP
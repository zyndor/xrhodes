//
// Nuclear Heart Games
// XRhodes
//
// AutoMatrix.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    24/10/2012
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_AUTOMATRIX_HPP
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
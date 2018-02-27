//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/ProjectionHelper.hpp"
#include "XR/debug.hpp"
#include <cmath>
#include <cstring>

namespace XR
{

//==============================================================================
void ProjectionHelper::CalculateOrthographic(float left, float right, float bottom,
  float top, float zNear, float zFar, float matrix[kNumMatrixElems])
{
  XR_ASSERT(ProjectionHelper, left != right);
  XR_ASSERT(ProjectionHelper, top != bottom);
  XR_ASSERT(ProjectionHelper, zFar > zNear);

  float dx(1.0f / (right - left));
  float dy(1.0f / (top - bottom));
  float dz(1.0f / (zNear - zFar));	// simplified -dz

  float*  pWrite = matrix;
  *pWrite = 2.0f * dx;
  *++pWrite = 0.0f;
  *++pWrite = 0.0f;
  *++pWrite = 0.0f;
  *++pWrite = 0.0f;
  *++pWrite = 2.0f * dy;
  *++pWrite = 0.0f;
  *++pWrite = 0.0f;
  *++pWrite = 0.0f;
  *++pWrite = 0.0f;
  *++pWrite = 2.0f * dz;
  *++pWrite = 0.0f;
  *++pWrite = (left + right) * -dx;
  *++pWrite = (top + bottom) * -dy;
  *++pWrite = (zFar + zNear) * dz;
  *++pWrite = 1.0f;
}

//==============================================================================
void ProjectionHelper::CalculateOrthographic(float width, float height, float zNear,
  float zFar, float matrix[kNumMatrixElems])
{
  XR_ASSERT(ProjectionHelper, width > 0.0f);
  XR_ASSERT(ProjectionHelper, height > 0.0f);
  XR_ASSERT(ProjectionHelper, zFar > zNear);

  CalculateOrthographic(.0f, width, -height, .0f, zNear, zFar, matrix);
}

//==============================================================================
void ProjectionHelper::CalculateOrthographic(float size, float zNear, float zFar,
  float matrix[kNumMatrixElems])
{
  XR_ASSERT(ProjectionHelper, size >= 0.0f);

  size *= .5f;
  CalculateOrthographic(-size, size, size, -size, zNear, zFar, matrix);
}

//==============================================================================
void ProjectionHelper::CalculatePerspective(float verticalFOV, float aspectRatio,
  float zNear, float zFar, float matrix[kNumMatrixElems],
  float* pTanVerticalFOVHalf)
{
  float tanVerticalFOVHalf = tanf(verticalFOV * 0.5f);
  if(pTanVerticalFOVHalf)
  {
    *pTanVerticalFOVHalf = tanVerticalFOVHalf;
  }

  float f(1.0f / tanVerticalFOVHalf);
  float dz(1.0f / (zNear - zFar));

  float*  pWrite = matrix;
  *pWrite = f / aspectRatio;
  *++pWrite = 0.0f;
  *++pWrite = 0.0f;
  *++pWrite = 0.0f;
  *++pWrite = 0.0f;
  *++pWrite = f;
  *++pWrite = 0.0f;
  *++pWrite = 0.0f;
  *++pWrite = 0.0f;
  *++pWrite = 0.0f;
  *++pWrite = (zFar + zNear) * dz;
  *++pWrite = -1.0f;
  *++pWrite = 0.0f;
  *++pWrite = 0.0f;
  *++pWrite = 2.0f * zFar * zNear * dz;
  *++pWrite = 0.0f;
}

} // XR

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "ProjectionHelpers.hpp"
#include "types.hpp"  // XR_ASSERT, temporary
#include <cmath>
#include <cstring>

namespace XR
{

//==============================================================================
void ProjectionHelpers::CalculateOrtho(float left, float right, float top,
  float bottom, float zNear, float zFar, float(&arData)[16])
{
  XR_ASSERT(ProjectionHelpers, left != right);
  XR_ASSERT(ProjectionHelpers, top != bottom);
  XR_ASSERT(ProjectionHelpers, zFar > 0.0f);

  float dx(1.0f / (right - left));
  float dy(1.0f / (bottom - top));  // simplified -dy
  float dz(1.0f / (zNear - zFar));	// simplified -dz

  float*  pWrite = arData;
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
  *++pWrite = (top + bottom) * dy;
  *++pWrite = (zFar + zNear) * dz;
  *++pWrite = 1.0f;
}

//==============================================================================
void ProjectionHelpers::CalculateOrtho(float width, float height, float zNear,
  float zFar, float(&arData)[16])
{
  XR_ASSERT(ProjectionHelpers, width > 0.0f);
  XR_ASSERT(ProjectionHelpers, height > 0.0f);
  XR_ASSERT(ProjectionHelpers, zFar > 0.0f);
  XR_ASSERT(ProjectionHelpers, zNear > 0.0f);

  float dx(1.0f / width);
  float dy(1.0f / height);
  float dz(1.0f / (zNear - zFar));

  float*  pWrite = arData;
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
  *++pWrite = width * -dx;
  *++pWrite = height * dy;
  *++pWrite = (zFar + zNear) * dz;
  *++pWrite = 1.0f;
}

//==============================================================================
void ProjectionHelpers::CalculatePerspective(float verticalFOV, float aspectRatio,
  float zNear, float zFar, float(&arData)[16], float* pTanVerticalFOVHalf)
{
  float tanVerticalFOVHalf = tanf(verticalFOV * 0.5f);
  if(pTanVerticalFOVHalf != 0)
  {
    *pTanVerticalFOVHalf = tanVerticalFOVHalf;
  }

  float f(1.0f / tanVerticalFOVHalf);
  float dz(1.0f / (zNear - zFar));

  float*  pWrite = arData;
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
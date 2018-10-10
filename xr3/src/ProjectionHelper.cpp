//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/ProjectionHelper.hpp"
#include "xr/debug.hpp"
#include <cmath>
#include <cstring>

namespace xr
{

//==============================================================================
void ProjectionHelper::CalculateOrthographic(float left, float right, float bottom,
  float top, float zNear, float zFar, float outMatrix[kNumMatrixElems])
{
  XR_ASSERT(ProjectionHelper, left != right);
  XR_ASSERT(ProjectionHelper, top != bottom);
  XR_ASSERT(ProjectionHelper, zFar != zNear);

  float dx = 1.0f / (right - left);
  float dy = 1.0f / (top - bottom);
  float dz = -1.0f / (zFar - zNear); // NOTE: pre-negated.

  float*  writep = outMatrix;
  *writep = 2.0f * dx;
  *++writep = 0.0f;
  *++writep = 0.0f;
  *++writep = 0.0f;
  *++writep = 0.0f;
  *++writep = 2.0f * dy;
  *++writep = 0.0f;
  *++writep = 0.0f;
  *++writep = 0.0f;
  *++writep = 0.0f;
  *++writep = 2.0f * dz;
  *++writep = 0.0f;
  *++writep = (left + right) * -dx;
  *++writep = (top + bottom) * -dy;
  *++writep = (zFar + zNear) * dz;
  *++writep = 1.0f;
}

//==============================================================================
void ProjectionHelper::CalculateOrthographic(float width, float height, float zNear,
  float zFar, float outMatrix[kNumMatrixElems])
{
  XR_ASSERT(ProjectionHelper, width > 0.0f);
  XR_ASSERT(ProjectionHelper, height > 0.0f);
  XR_ASSERT(ProjectionHelper, zFar != zNear);

  CalculateOrthographic(.0f, width, -height, .0f, zNear, zFar, outMatrix);
}

//==============================================================================
void ProjectionHelper::CalculateOrthographic(float size, float zNear, float zFar,
  float outMatrix[kNumMatrixElems])
{
  XR_ASSERT(ProjectionHelper, size >= 0.0f);

  size *= .5f;
  CalculateOrthographic(-size, size, -size, size, zNear, zFar, outMatrix);
}

//==============================================================================
void ProjectionHelper::CalculatePerspective(float verticalFOV, float aspectRatio,
  float zNear, float zFar, float outMatrix[kNumMatrixElems],
  float* outTanVerticalFovHalf)
{
  float tanVerticalFOVHalf = tanf(verticalFOV * 0.5f);
  if(outTanVerticalFovHalf)
  {
    *outTanVerticalFovHalf = tanVerticalFOVHalf;
  }

  float f = 1.0f / tanVerticalFOVHalf;
  float dz = -1.0f / (zFar - zNear); // NOTE: pre-negated.

  float*  writep = outMatrix;
  *writep = f / aspectRatio;
  *++writep = 0.0f;
  *++writep = 0.0f;
  *++writep = 0.0f;
  *++writep = 0.0f;
  *++writep = f;
  *++writep = 0.0f;
  *++writep = 0.0f;
  *++writep = 0.0f;
  *++writep = 0.0f;
  *++writep = (zFar + zNear) * dz;
  *++writep = -1.0f;
  *++writep = 0.0f;
  *++writep = 0.0f;
  *++writep = 2.0f * zFar * zNear * dz;
  *++writep = 0.0f;
}

} // XR

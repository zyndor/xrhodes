//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_PROJECTIONHELPERS_HPP
#define XR_PROJECTIONHELPERS_HPP

#include "types.hpp"

namespace XR
{

//==============================================================================
///@brief A collection of helper functions required to calculate projections.
class ProjectionHelpers
{
  XR_NONOBJECT_DECL(ProjectionHelpers)

public:
  static void CalculateOrtho(float left, float right, float bottom, float top,
    float zNear, float zFar, float (&arData)[16]);

  static void CalculateOrtho(float width, float height, float zNear, float zFar,
    float(&arData)[16]);

  static void CalculatePerspective(float verticalFOV, float aspectRatio,
    float zNear, float zFar, float (&arData)[16], float* pTanVerticalFOVHalf = 0);
};

} // XR

#endif// XR_PROJECTIONHELPERS_HPP
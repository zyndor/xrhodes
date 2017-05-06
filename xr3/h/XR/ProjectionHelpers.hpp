//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_PROJECTIONHELPERS_HPP
#define XR_PROJECTIONHELPERS_HPP

#include <XR/fundamentals.hpp>

namespace XR
{

//==============================================================================
///@brief A collection of helper functions required to calculate projections.
class ProjectionHelpers
{
  XR_NONOBJECT_DECL(ProjectionHelpers)

public:
  ///@brief Calculates an orthographic projection from intended @a left,
  /// @a right, @a bottom, @a top coordinates, and distances of the near and
  /// far panes.
  static void CalculateOrtho(float left, float right, float bottom, float top,
    float zNear, float zFar, float (&arData)[16]);

  ///@brief Calculates an orthographic projection from intendend @a width and
  /// @a height, meaning that the top left of the resultant projection is
  /// (0; 0), x goes right and y goes down.
  static void CalculateOrtho(float width, float height, float zNear, float zFar,
    float(&arData)[16]);

  ///@brief Calculates a perspective projection with the given vertical
  /// field of view, aspect radio and distances of near and far viewing
  /// panes. The tangent of the half vertical FOV may be stored in
  /// @a pTanVerticalFOVHalf
  static void CalculatePerspective(float verticalFOV, float aspectRatio,
    float zNear, float zFar, float (&arData)[16], float* pTanVerticalFOVHalf = nullptr);
};

} // XR

#endif// XR_PROJECTIONHELPERS_HPP
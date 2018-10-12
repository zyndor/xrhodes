#ifndef XR_PROJECTIONHELPER_HPP
#define XR_PROJECTIONHELPER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/types/fundamentals.hpp"

namespace xr
{

//==============================================================================
///@brief A collection of helper functions required to calculate projections.
class ProjectionHelper
{
  XR_NONOBJECT_DECL(ProjectionHelper)

public:
  enum { kNumMatrixElems = 16 };

  ///@brief Calculates an orthographic projection from intended @a left,
  /// @a right, @a bottom, @a top coordinates, and distances of the near and
  /// far panes.
  ///@note @a outMatrix must point to an array of at least kNumMatrixElems elements.
  static void CalculateOrthographic(float left, float right, float bottom,
    float top, float zNear, float zFar, float outMatrix[kNumMatrixElems]);

  ///@brief Calculates an orthographic projection from intendend @a width and
  /// @a height, meaning that the top left of the resultant projection is
  /// (0; 0), X goes right and Y goes up. i.e. UI layout should go in the
  /// negative Y.
  ///@note @a outMatrix must point to an array of at least kNumMatrixElems elements.
  static void CalculateOrthographic(float width, float height, float zNear,
    float zFar, float outMatrix[kNumMatrixElems]);

  ///@brief Calculates orthographics projection equilateral in X and Y, i.e. left /
  /// bottom and right / top clipping planes are all at + / - @a size / 2.
  static void CalculateOrthographic(float size, float zNear, float zFar,
    float outMatrix[kNumMatrixElems]);

  ///@brief Calculates a perspective projection with the given vertical field
  /// of view, aspect radio and distances of near and far viewing planes. The
  /// tangent of the half vertical field of view may be stored in
  /// @a outTanVerticalFovHalf.
  ///@note @a outMatrix must point to an array of at least kNumMatrixElems elements.
  static void CalculatePerspective(float verticalFOV, float aspectRatio,
    float zNear, float zFar, float outMatrix[kNumMatrixElems],
    float* outTanVerticalFovHalf = nullptr);
};

} // XR

#endif// XR_PROJECTIONHELPER_HPP

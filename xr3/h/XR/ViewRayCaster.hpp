#ifndef XR_VIEWRAYCASTER_HPP
#define XR_VIEWRAYCASTER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Gfx.hpp"
#include "Ray.hpp"
#include "Matrix.hpp"
#include "Vector3.hpp"
#include <cmath>

namespace XR
{

//==============================================================================
///@brief Facilitates the calculation of rays based on a world space viewer
/// transform, a z value for the near clipping plane and the tangent of the
/// half of the vertical field of view. The rays intersect the near clipping
/// plane at the given screen space or normalized screen space coordinates.
struct ViewRayCaster
{
  Matrix viewer;
  float zNear;
  float tanHalfVerticalFov;

  ///@brief Sets the vertical field of view in radians, storing it as the
  /// tangent of its half.
  void SetVerticalFov(float fovRadians)
  {
    tanHalfVerticalFov = std::tan(fovRadians * .5f);
  }

  ///@brief Calculates a ray intersecting the near clipping plane at the given
  /// screen space coordinates. { 0; 0 } is top left. The ray is cast from the
  /// viewer's world space position and extends to infinity.
  ///@note The position and direction of the Ray is in world space.
  Ray GetViewRay(int x, int y) const
  {
    float nx = (static_cast<float>(x) / Gfx::GetWidth() - .5f) * 2.0f;
    float ny = (.5f - static_cast<float>(y) / Gfx::GetHeight()) * 2.0f;
    return GetViewRay(nx, ny);
  }

  ///@brief Calculates a ray intersecting the near clipping plane at the given
  /// normalized screen space coordinates. { -1.0; -1.0 } is bottom left. The ray
  /// is cast from the viewer's world space position and extends to infinity.
  ///@note The position and direction of the Ray is in world space.
  Ray GetViewRay(float nx, float ny) const
  {
    float hProj = zNear * tanHalfVerticalFov;
    float wProj = (hProj * Gfx::GetWidth()) / Gfx::GetHeight();

    Vector3 zNearHit = viewer.GetColumn(Vector3::Z) * -zNear +
      viewer.GetColumn(Vector3::X).Normalise(wProj * nx) +
      viewer.GetColumn(Vector3::Y).Normalise(hProj * ny);
    zNearHit.Normalise();

    return Ray(viewer.t, zNearHit, std::numeric_limits<float>::max());
  }
};

} // XR

#endif //XR_VIEWRAYCASTER_HPP

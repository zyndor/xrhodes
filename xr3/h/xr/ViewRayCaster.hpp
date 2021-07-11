#ifndef XR_VIEWRAYCASTER_HPP
#define XR_VIEWRAYCASTER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
//==============================================================================
#include "Gfx.hpp"  // TODO: remove
#include "xr/math/Ray.hpp"
#include "xr/math/Matrix.hpp"
#include "xr/math/Vector3.hpp"
#include <cmath>

namespace xr
{

//==============================================================================
///@brief Facilitates the calculation of rays based on a world space viewer
/// transform, a z value for the near clipping plane and the tangent of the
/// half of the vertical field of view. The rays intersect the near clipping
/// plane at the given screen space or normalized screen space coordinates.
class ViewRayCaster
{
public:
  // static
  [[deprecated("supply aspect ratio; the Gfx dependency is being removed.")]]
  static Ray GetViewRay(float nx, float ny, Matrix const& viewer, float zNear,
    float tanHalfVerticalFov)
  {
    return GetViewRay(nx, ny, viewer, Gfx::GetLogicalAspectRatio(), zNear,
      tanHalfVerticalFov);
  }

  static Ray GetViewRay(float nx, float ny, Matrix const& viewer, float aspectRatio,
    float zNear, float tanHalfVerticalFov)
  {
    float hProj = zNear * tanHalfVerticalFov;
    float wProj = hProj * aspectRatio;

    Vector3 zNearHit = Vector3(viewer.linear + Matrix::ZX) * -zNear +
      Vector3(viewer.linear + Matrix::XX).Normalise(wProj * nx) +
      Vector3(viewer.linear + Matrix::YX).Normalise(hProj * ny);
    zNearHit.Normalise();

    return Ray(viewer.t, zNearHit, std::numeric_limits<float>::max());
  }

  // data
  Matrix viewer;
  float aspectRatio;
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
    float nx = (static_cast<float>(x) / Gfx::GetLogicalWidth() - .5f) * 2.0f;
    float ny = (.5f - static_cast<float>(y) / Gfx::GetLogicalHeight()) * 2.0f;
    return GetViewRay(nx, ny);
  }

  ///@brief Calculates a ray intersecting the near clipping plane at the given
  /// normalized screen space coordinates. { -1.0; -1.0 } is bottom left. The ray
  /// is cast from the viewer's world space position and extends to infinity.
  ///@note The position and direction of the Ray is in world space.
  Ray GetViewRay(float nx, float ny) const
  {
    return GetViewRay(nx, ny, viewer, aspectRatio, zNear, tanHalfVerticalFov);
  }
};

} // XR

#endif //XR_VIEWRAYCASTER_HPP

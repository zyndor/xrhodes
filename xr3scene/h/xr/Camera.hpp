#ifndef XR_CAMERA_HPP
#define XR_CAMERA_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Component.hpp"
#include "xr/math/Matrix4.hpp"

namespace xr
{

class Matrix;
class Ray;

//==============================================================================
///@brief Camera provides facilities for setting projection and view transforms,
/// and cast rays using these.
class XR_COMPONENT_DECL(Camera)
{
public:
  // structors
  Camera();

  // general
  [[nodiscard]] Camera* Clone() const override;

  ///@brief Sets up perspective projection.
  ///@param verticalFov Vertical field of view in radians.
  ///@param aspectRatio The ratio of the projection width to (/ over) height.
  ///@param zNear The distance of the near clipping pane from the camera.
  ///@param zFar The distance of the far clipping pane from the camera.
  void  SetPerspective(float verticalFovRadians, float aspectRatio, float zNear,
    float zFar);

  ///@brief Sets up orthographic projection.
  ///@param width The width of the projection.
  ///@param height The height of the projection.
  ///@param zNear The distance of the near clipping pane from the camera.
  ///@param zFar The distance of the far clipping pane from the camera.
  void  SetOrthographic(float width, float height, float zNear, float zFar);

  ///@brief Convenience method to get the viewer's world space transform.
  ///@return The world space transform of its owner.
  Matrix  GetViewerTransform() const;

  ///@return The position of the near clipping plane.
  float GetNearZ() const;

  ///@return The position of the far clipping plane.
  float GetFarZ() const;

  ///@return The projection matrix used by this camera.
  Matrix4 const& GetProjectionMatrix() const;

  ///@return The perspective multiple, which, multiplied by the height of the projection
  /// will yield the view space z position at which objects appear at scale 1.0.
  ///@note The camera must be set up for perspective projection.
  float GetPerspectiveMultiple() const;

  ///@brief Alters the size of the projection.
  ///@note The field of view is retained (which should only matter for perspective
  /// projection).
  ///@note The default is perspective projection.
  void  Reshape(float width, float height);

  ///@brief Returns a ray starting from the owner Entity's worldspace
  /// position, intersecting the near clipping pane at the normalized
  /// coordinates {@a nx, @a ny }. {-1, -1} is bottom left, {+1, +1} is
  /// top right.
  Ray GetViewRay(float nx, float ny) const;

  ///@brief Applies the projection matrix and the worldspace transform of
  /// the Camera's owner as a view matrix.
  void Apply();

protected:
  // data
  bool  m_isPerspective;
  float m_zNear;  // depth of the near clipping plane
  float m_zFar; // depth of the far clipping plane
  float m_aspectRatio;  // ratio of width to height; width = activeSize * aspectRatio;
  float m_verticalFovRadians; // vertical field of view in radians;
  float m_tanHalfVerticalFov; // tangent of the half vertical field of view;
  float m_perspectiveMultiple;

  Matrix4 m_projection;
};

}

#endif  //XR_CAMERA_HPP

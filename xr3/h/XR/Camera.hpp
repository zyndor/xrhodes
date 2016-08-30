//
// Nuclear Heart Interactive
// XRhodes
//
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
//
// copyright (c) 2011 - 2016. All rights reserved.
//
//==============================================================================
#ifndef XR_CAMERA_HPP
#define XR_CAMERA_HPP

#include "Entity.hpp"
#include "Ray.hpp"

namespace XR
{

//==============================================================================
class XR_COMPONENT_DECL(Camera)
{
public:
  // structors
  Camera();
  ~Camera();

  // general
  Camera* Clone() const;

  ///@brief Sets up perspective projection.
  ///@param verticalFov Vertical field of view in radians.
  ///@param aspectRatio The ratio of the projection width to (/ over) height.
  ///@param zNear The distance of the near clipping pane from the camera.
  ///@param zFar The distance of the far clipping pane from the camera.
  void    SetPerspective(float verticalFov, float aspectRatio, float zNear, float zFar);

  ///@brief Sets up orthographic projection.
  ///@param width The width of the projection.
  ///@param height The height of the projection.
  ///@param zNear The distance of the near clipping pane from the camera.
  ///@param zFar The distance of the far clipping pane from the camera.
  void    SetOrtho(float width, float height, float zNear, float zFar);

  Matrix  GetView() const;
  float   GetNearZ() const;
  float   GetFarZ() const;
  float   GetPerspectiveMultiple(float height) const;

  ///@brief Alters the size of the projection.
  ///@note SetPerspective() or SetOrtho() is supposed to be called beforehand.
  void    Reshape(float width, float height);

  ///@brief Returns a ray starting from the owner Entity's worldspace
  /// position, intersecting the near clipping pane at the normalized
  /// coordinates {@a nx, @a ny }. {-1, -1} is top left, {+1, +1} is
  /// bottom right. 
  Ray     GetViewRay(float nx, float ny) const;

  ///@brief Applies the perspective matrix and the worldspace transform of
  /// the Camera's owner as a view matrix to the Renderer.
  void    Apply();

protected:
  bool    m_isPerspective;
  float   m_arPerspectiveMatrix[16];
  float   m_zNear;
  float   m_zFar;
  float   m_verticalFov;
  float   m_aspectRatio;
};

} // XR

#endif  //XR_RENDERER_HPP
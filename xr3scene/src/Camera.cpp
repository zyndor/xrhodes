//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Camera.hpp"
#include "xr/Entity.hpp"
#include "xr/ViewRayCaster.hpp"
#include "xr/ProjectionHelper.hpp"
#include "xr/Transforms.hpp"
#include "xr/math/Matrix.hpp"
#include "xr/math/Ray.hpp"
#include "xr/math/Matrix4.hpp"
#include "xr/debug.hpp"

namespace xr
{

//==============================================================================
Camera::Camera()
{
  SetPerspective(float(kPi * .25f), .75f, 10.0f, 1000.0f);
}

//==============================================================================
Camera* Camera::Clone() const
{
  Camera* pClone = new Camera();
  pClone->m_isPerspective = m_isPerspective;
  pClone->m_zNear = m_zNear;
  pClone->m_zFar = m_zFar;
  pClone->m_orthoSize = m_orthoSize;
  pClone->m_aspectRatio = m_aspectRatio;
  pClone->m_verticalFovRadians = m_verticalFovRadians;
  pClone->m_tanHalfVerticalFov = m_tanHalfVerticalFov;
  return pClone;
}

//==============================================================================
void Camera::SetPerspective(float verticalFovRadians, float aspectRatio,
  float zNear, float zFar)
{
  XR_ASSERT(Camera, verticalFovRadians > .0f);
  XR_ASSERT(Camera, verticalFovRadians < M_PI);
  XR_ASSERT(Camera, aspectRatio > .0f);
  XR_ASSERT(Camera, zNear < zFar);
  m_isPerspective = true;
  m_zNear = zNear;
  m_zFar = zFar;
  //m_orthoSize not set
  m_aspectRatio = aspectRatio;
  m_activeSize = 1.0f;
  m_verticalFovRadians = verticalFovRadians;

  ProjectionHelper::CalculatePerspective(verticalFovRadians, aspectRatio,
    m_zNear, m_zFar, m_projection.data, &m_tanHalfVerticalFov);
}

//==============================================================================
void Camera::SetOrthographic(float width, float height, float zNear, float zFar)
{
  XR_ASSERT(Camera, zFar > .0f);
  XR_ASSERT(Camera, zNear < zFar);
  m_isPerspective = false;
  m_zNear = zNear;
  m_zFar = zFar;
  m_orthoSize = height;
  m_aspectRatio = width / height;
  m_activeSize = height;
  //m_verticalFovRadians not set

  ProjectionHelper::CalculateOrthographic(width, height, zNear, zFar,
    m_projection.data);
}

//==============================================================================
Matrix  Camera::GetViewerTransform() const
{
  return GetOwner()->GetWorldTransform();
}

//==============================================================================
float Camera::GetNearZ() const
{
  return m_zNear;
}

//==============================================================================
float Camera::GetFarZ() const
{
  return m_zFar;
}

//==============================================================================
void Camera::Reshape(float width, float height)
{
  if(m_isPerspective)
  {
    SetPerspective(m_verticalFovRadians, width / height, m_zNear, m_zFar);
  }
  else
  {
    SetOrthographic(width, height, m_zNear, m_zFar);
  }
}

//==============================================================================
Ray Camera::GetViewRay(float nx, float ny) const
{
  return ViewRayCaster::GetViewRay(nx, ny, GetViewerTransform(), m_aspectRatio,
    m_zNear, m_tanHalfVerticalFov);
}

//==============================================================================
void Camera::Apply()
{
  Transforms::Updater().SetViewerTransform(GetViewerTransform()).
    SetProjection(m_projection, m_zNear, m_zFar, m_tanHalfVerticalFov);
}

//==============================================================================
float Camera::GetPerspectiveMultiple() const
{
  return m_activeSize * .5f * m_tanHalfVerticalFov;
}

}

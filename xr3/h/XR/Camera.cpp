//
// Nuclear Heart Interactive
// XRhodes
//
// copyright (c) 2011 - 2016. All rights reserved.
//
//==============================================================================
#include "Camera.hpp"
#include "ProjectionHelpers.hpp"
#include "Renderer.hpp"

namespace XR
{

//==============================================================================
Camera::Camera()
{
  SetPerspective(float(M_PI * .25f), .75f, 10.0f, 1000.0f);
}

//==============================================================================
Camera::~Camera()
{
}

//==============================================================================
Camera* Camera::Clone() const
{
  Camera* pClone = new Camera();
  memcpy(&pClone->m_arPerspectiveMatrix, &m_arPerspectiveMatrix, sizeof(Camera) - sizeof(Entity*));
  return pClone;
}

//==============================================================================
void Camera::SetPerspective(float verticalFieldOfView, float aspectRatio, float zNear, float zFar)
{
  XR_ASSERT(Camera, verticalFieldOfView > .0f);
  XR_ASSERT(Camera, verticalFieldOfView < M_PI);
  XR_ASSERT(Camera, aspectRatio > .0f);
  XR_ASSERT(Camera, zNear < zFar);
  m_isPerspective = true;
  m_zNear = zNear;
  m_zFar = zFar;
  m_verticalFov = verticalFieldOfView;
  m_aspectRatio = aspectRatio;

  ProjectionHelpers::CalculatePerspective(m_verticalFov, aspectRatio, m_zNear, m_zFar,
    m_arPerspectiveMatrix);
}

//==============================================================================
void Camera::SetOrtho(float width, float height, float zNear, float zFar)
{
  XR_ASSERT(Camera, zFar > .0f);
  XR_ASSERT(Camera, zNear < zFar);
  m_isPerspective = false;
  m_zNear = zNear;
  m_zFar = zFar;
  m_verticalFov = .0f;
  m_aspectRatio = width / height;

  ProjectionHelpers::CalculateOrtho(width, height, zNear, zFar,
    m_arPerspectiveMatrix);
}

//==============================================================================
Matrix  Camera::GetView() const
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
    SetPerspective(m_verticalFov, width / height, m_zNear, m_zFar);
  }
  else
  {
    SetOrtho(width, height, m_zNear, m_zFar);
  }
}

//==============================================================================
Ray Camera::GetViewRay(float nx, float ny) const
{
  Matrix mView = GetView();

  Vector3 vForward(mView.arLinear + MZX);
  //vForward.Normalise();

  Vector3 vRight(mView.arLinear + MXX);
  Vector3 vUp(mView.arLinear + MYX);

  float hProj(m_zNear * tanf(m_verticalFov * .5f));
  float wProj(hProj * m_aspectRatio);

  vRight.Normalise(wProj);
  vUp.Normalise(hProj);

  Vector3 zNearHit((vForward * m_zNear) + (vRight * nx) + (vUp * ny));
  zNearHit.Normalise();

  return Ray(mView.t, zNearHit, std::numeric_limits<float>::max());
}

//==============================================================================
void Camera::Apply()
{
  Renderer::SetPerspMatrix(m_arPerspectiveMatrix);
  Renderer::SetViewMatrix(GetView());
}

//==============================================================================
float Camera::GetPerspectiveMultiple(float height) const
{
  return height * .5f * tanf(m_verticalFov * .5f);
}

} // XR
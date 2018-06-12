//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/Transforms.hpp"
#include "xr/ViewRayCaster.hpp"
#include "xr/Gfx.hpp"
#include "xr/ProjectionHelper.hpp"
#include "xr/Matrix4.hpp"
#include "xr/debug.hpp"
#include <vector>
#include <memory>

namespace xr
{
namespace
{

//==============================================================================
class TransformsImpl
{
public:
  TransformsImpl()
  {
    m_modelStack.reserve(16);
  }

  ~TransformsImpl()
  {
    Gfx::Release(m_xruNormal);
    Gfx::Release(m_xruModelViewProjection);
    Gfx::Release(m_xruViewProjection);
    Gfx::Release(m_xruModelView);
    Gfx::Release(m_xruProjection);
    Gfx::Release(m_xruView);
    Gfx::Release(m_xruModel);
  }

  bool IsUpdateInProgress() const
  {
    return m_dirtyFlags != 0;
  }

  void SetProjection(Matrix4 const& matrix, float zNear, float zFar,
    float tanHalfVerticalFov)
  {
    m_projection = matrix;
    m_tanHalfVerticalFov = tanHalfVerticalFov;
    m_zNear = zNear;
    m_zFar = zFar;
    m_dirtyFlags |= PROJECTION_DIRTY;
  }

  void SetOrthographicProjection(float left, float right, float bottom, float top,
    float zNear, float zFar)
  {
    XR_ASSERT(Transforms, zNear < zFar);
    if (zNear == .0f)
    {
      zNear = -zFar;
    }

    ProjectionHelper::CalculateOrthographic(left, right, bottom, top, zNear, zFar,
      m_projection.data);
    m_zNear = zNear;
    m_zFar = zFar;
    m_tanHalfVerticalFov = .0f;
    m_dirtyFlags |= PROJECTION_DIRTY;
  }

  void SetPerspectiveProjection(float verticalFov, float aspectRatio, float zNear,
    float zFar)
  {
    XR_ASSERT(Transforms, zNear < zFar);
    XR_ASSERT(Transforms, aspectRatio > .0f);

    ProjectionHelper::CalculatePerspective(verticalFov, aspectRatio, zNear, zFar,
      m_projection.data, &m_tanHalfVerticalFov);
    m_zNear = zNear;
    m_zFar = zFar;
    m_dirtyFlags |= PROJECTION_DIRTY;
  }

  void SetView(Matrix4 const& matrix)
  {
    m_view = matrix;
    m_dirtyFlags |= VIEW_DIRTY;
  }

  void SetViewerTransform(Matrix m)
  {
    m.Invert();
    m.t = m.RotateVec(-m.t);
    m_view.Import(m);
    m_dirtyFlags |= VIEW_DIRTY;
  }

  void SetModel(const Matrix& m)
  {
    m_modelStack.clear();
    m_modelStack.push_back(m);
    m_dirtyFlags |= MODEL_DIRTY;
  }

  void PushModel(const Matrix& m)
  {
    if (m_modelStack.empty())
    {
      m_modelStack.push_back(m);
    }
    else
    {
      m_modelStack.push_back(m * m_modelStack.back());
    }
    m_dirtyFlags |= MODEL_DIRTY;
  }

  void PopModel()
  {
    m_modelStack.pop_back();
    m_dirtyFlags |= MODEL_DIRTY;
  }

  void Update()
  {
    if (CheckAnyMaskBits(m_dirtyFlags, MODEL_DIRTY | VIEW_DIRTY | PROJECTION_DIRTY))
    {
      Matrix4 model;
      model.Import(m_modelStack.back());
      if (CheckAnyMaskBits(m_dirtyFlags, MODEL_DIRTY))
      {
        Gfx::SetUniform(m_xruModel, 1, model.data);
      }

      if (CheckAnyMaskBits(m_dirtyFlags, VIEW_DIRTY))
      {
        Gfx::SetUniform(m_xruView, 1, m_view.data);
      }

      if (CheckAnyMaskBits(m_dirtyFlags, PROJECTION_DIRTY))
      {
        Gfx::SetUniform(m_xruProjection, 1, m_projection.data);
      }

      Matrix4 modelViewProjection;
      if (CheckAnyMaskBits(m_dirtyFlags, MODEL_DIRTY | VIEW_DIRTY))
      {
        model.Multiply(m_view, modelViewProjection);  // hijack mvp for the model view calculation.
        Gfx::SetUniform(m_xruModelView, 1, modelViewProjection.data);

        Matrix normal;
        modelViewProjection.Export(normal);
        normal.Invert();
        normal.Transpose();
        Gfx::SetUniform(m_xruNormal, normal.linear);
      }

      Matrix4 viewProjection;
      m_view.Multiply(m_projection, viewProjection);
      Gfx::SetUniform(m_xruViewProjection, 1, viewProjection.data);

      model.Multiply(viewProjection, modelViewProjection);
      Gfx::SetUniform(m_xruModelViewProjection, 1, modelViewProjection.data);

      m_dirtyFlags = 0;
    }
  }

  void GetModelMatrix(Matrix& m) const
  {
    m = m_modelStack.back();
  }

  void GetViewerTransform(Matrix& m) const
  {
    m_view.Export(m);
    m.Invert();
    m.t = -m.t;
  }

  void GetView(Matrix4& matrix) const
  {
    matrix = m_view;
  }

  void GetProjection(Matrix4& matrix) const
  {
    matrix = m_projection;
  }

  float GetPerspectiveMultiple() const
  {
    return m_tanHalfVerticalFov * (Gfx::GetHeight() / 2);
  }

  float GetZNearClippingPlane() const
  {
    return m_zNear;
  }

  float GetZFarClippingPlane() const
  {
    return m_zFar;
  }

  ViewRayCaster GetViewRayCaster() const
  {
    xr::Matrix viewer;
    GetViewerTransform(viewer);

    return ViewRayCaster{ viewer, m_zNear, m_tanHalfVerticalFov };
  }

private:
  // types
  enum DirtyFlags
  {
    MODEL_DIRTY = 0x01,
    VIEW_DIRTY = 0x02,
    PROJECTION_DIRTY = 0x04
  };

  // data
  Gfx::UniformHandle m_xruModel = Gfx::CreateUniform("xruModel", Gfx::UniformType::Mat4);
  Gfx::UniformHandle m_xruView = Gfx::CreateUniform("xruView", Gfx::UniformType::Mat4);
  Gfx::UniformHandle m_xruProjection = Gfx::CreateUniform("xruProjection", Gfx::UniformType::Mat4);

  Gfx::UniformHandle m_xruModelView = Gfx::CreateUniform("xruModelView", Gfx::UniformType::Mat4);
  Gfx::UniformHandle m_xruViewProjection = Gfx::CreateUniform("xruViewProjection", Gfx::UniformType::Mat4);
  Gfx::UniformHandle m_xruModelViewProjection = Gfx::CreateUniform("xruModelViewProjection", Gfx::UniformType::Mat4);

  Gfx::UniformHandle m_xruNormal = Gfx::CreateUniform("xruNormal", Gfx::UniformType::Mat3);

  std::vector<Matrix> m_modelStack;
  Matrix4 m_view;
  Matrix4 m_projection;
  uint8_t m_dirtyFlags = 0;

  float m_zNear;
  float m_zFar;
  float m_tanHalfVerticalFov;
};

TransformsImpl* s_impl;

} //

//==============================================================================
Transforms::Updater::Updater()
{
  XR_ASSERT(Transforms, s_impl);
  XR_ASSERTMSG(Transforms, !s_impl->IsUpdateInProgress(),
    ("An update was already in progress; this might cause unexpected results."));
}

//==============================================================================
Transforms::Updater::~Updater()
{
  XR_ASSERT(Transforms, s_impl);
  s_impl->Update();
}

//==============================================================================
Transforms::Updater&
  Transforms::Updater::SetModel(const Matrix& m)
{
  s_impl->SetModel(m);
  return *this;
}

//==============================================================================
Transforms::Updater&
  Transforms::Updater::PushModel(const Matrix& m)
{
  s_impl->PushModel(m);
  return *this;
}

//==============================================================================
Transforms::Updater&
  Transforms::Updater::PopModel()
{
  s_impl->PopModel();
  return *this;
}

//==============================================================================
Transforms::Updater&
  Transforms::Updater::SetViewerTransform(Matrix const& m)
{
  s_impl->SetViewerTransform(m);
  return *this;
}

//==============================================================================
Transforms::Updater&
  Transforms::Updater::SetView(Matrix4 const& matrix)
{
  s_impl->SetView(matrix);
  return *this;
}

//==============================================================================
Transforms::Updater&
  Transforms::Updater::SetProjection(Matrix4 const& matrix,
    float zNear, float zFar, float tanHalfVerticalFov)
{
  s_impl->SetProjection(matrix, zNear, zFar, tanHalfVerticalFov);
  return *this;
}

//==============================================================================
Transforms::Updater&
  Transforms::Updater::SetOrthographicProjection(float left, float right,
  float bottom, float top, float zNear, float zFar)
{
  s_impl->SetOrthographicProjection(left, right, bottom, top, zNear, zFar);
  return *this;
}

//==============================================================================
Transforms::Updater&
  Transforms::Updater::SetPerspectiveProjection(float verticalFov,
  float aspectRatio, float zNear, float zFar)
{
  s_impl->SetPerspectiveProjection(verticalFov, aspectRatio, zNear, zFar);
  return *this;
}

//==============================================================================
Transforms::Updater&
  Transforms::Updater::SetPerspectiveProjection(float verticalFov,
    float zNear, float zFar)
{
  float aspectRatio = float(Gfx::GetWidth()) / float(Gfx::GetHeight());
  return SetPerspectiveProjection(verticalFov, aspectRatio, zNear, zFar);
}

//==============================================================================
void Transforms::Init()
{
  XR_ASSERTMSG(Transforms, !s_impl, ("Already initialised!"));
  s_impl = new TransformsImpl();

  Gfx::RegisterExitCallback([](void*, void*) {
    delete s_impl;
    s_impl = nullptr;
  }, nullptr);

  Updater().SetModel(Matrix::Identity()).
    SetViewerTransform(Matrix::Identity()).
    SetPerspectiveProjection(M_PI * .25f, .1f, 100.0f);
}

//==============================================================================
void  Transforms::GetModel(Matrix& m)
{
  s_impl->GetModelMatrix(m);
}

//==============================================================================
void  Transforms::GetViewerTransform(Matrix& m)
{
  s_impl->GetViewerTransform(m);
}

//==============================================================================
void  Transforms::GetView(Matrix4& matrix)
{
  s_impl->GetView(matrix);
}

//==============================================================================
void  Transforms::GetProjection(Matrix4& matrix)
{
  s_impl->GetProjection(matrix);
}

//==============================================================================
float Transforms::GetPerspectiveMultiple()
{
  return s_impl->GetPerspectiveMultiple();
}

//==============================================================================
float Transforms::GetZNearClippingPlane()
{
  return s_impl->GetZNearClippingPlane();
}

//==============================================================================
float Transforms::GetZFarClippingPlane()
{
  return s_impl->GetZFarClippingPlane();
}

//==============================================================================
ViewRayCaster Transforms::GetViewRayCaster()
{
  return s_impl->GetViewRayCaster();
}

} // XR

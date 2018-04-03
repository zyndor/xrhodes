//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Transforms.hpp"
#include "XR/Gfx.hpp"
#include "XR/ProjectionHelper.hpp"
#include "XR/Matrix4Helper.hpp"
#include "XR/debug.hpp"
#include <vector>
#include <memory>

namespace XR
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
    Gfx::Destroy(m_xruModelViewProjection);
    Gfx::Destroy(m_xruViewProjection);
    Gfx::Destroy(m_xruModelView);
    Gfx::Destroy(m_xruProjection);
    Gfx::Destroy(m_xruView);
    Gfx::Destroy(m_xruModel);
  }

  bool IsUpdateInProgress() const
  {
    return m_dirtyFlags != 0;
  }

  void SetProjection(float const matrix[16], float zNear, float zFar,
    float tanHalfVerticalFov)
  {
    memcpy(m_projection, matrix, sizeof(m_projection));
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
      m_projection);
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
      m_projection, &m_tanHalfVerticalFov);
    m_zNear = zNear;
    m_zFar = zFar;
    m_dirtyFlags |= PROJECTION_DIRTY;
  }

  void SetView(float const matrix[16])
  {
    memcpy(m_view, matrix, sizeof(m_view));
    m_dirtyFlags |= VIEW_DIRTY;
  }

  void SetViewerTransform(Matrix m)
  {
    m.Invert();
    m.t = m.RotateVec(-m.t);
    Matrix4Helper::FromMatrix(m, m_view);
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
      float model[16];
      Matrix4Helper::FromMatrix(m_modelStack.back(), model);
      if (CheckAnyMaskBits(m_dirtyFlags, MODEL_DIRTY))
      {
        Gfx::SetUniform(m_xruModel, 1, model);
      }

      if (CheckAnyMaskBits(m_dirtyFlags, VIEW_DIRTY))
      {
        Gfx::SetUniform(m_xruView, 1, m_view);
      }

      if (CheckAnyMaskBits(m_dirtyFlags, PROJECTION_DIRTY))
      {
        Gfx::SetUniform(m_xruProjection, 1, m_projection);
      }

      float mvp[16];
      if (CheckAnyMaskBits(m_dirtyFlags, MODEL_DIRTY | VIEW_DIRTY))
      {
        Matrix4Helper::Multiply(m_view, model, mvp);  // hitch a ride
        Gfx::SetUniform(m_xruModelView, 1, mvp);
      }

      float viewProjection[16];
      Matrix4Helper::Multiply(m_projection, m_view, viewProjection);
      Gfx::SetUniform(m_xruViewProjection, 1, viewProjection);

      Matrix4Helper::Multiply(viewProjection, model, mvp);
      Gfx::SetUniform(m_xruModelViewProjection, 1, mvp);

      m_dirtyFlags = 0;
    }
  }

  void GetModelMatrix(Matrix& m) const
  {
    m = m_modelStack.back();
  }

  void GetViewerTransform(Matrix& m) const
  {
    Matrix4Helper::ToMatrix(m_view, m);
    m.Invert();
    m.t = -m.t;
  }

  void GetView(float matrix[Transforms::kNumMatrixElems]) const
  {
    memcpy(matrix, m_view, sizeof(m_view));
  }

  void GetProjection(float matrix[Transforms::kNumMatrixElems]) const
  {
    memcpy(matrix, m_projection, sizeof(m_view));
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

  Ray GetViewRay(float nx, float ny) const
  {
    XR::Matrix viewer;
    GetViewerTransform(viewer);

    float hProj = m_zNear * m_tanHalfVerticalFov;
    float wProj = (hProj * Gfx::GetWidth()) / Gfx::GetHeight();

    Vector3 zNearHit = viewer.GetColumn(Vector3::Z) * -m_zNear +
      viewer.GetColumn(Vector3::X).Normalise(wProj * nx) +
      viewer.GetColumn(Vector3::Y).Normalise(hProj * ny);
    zNearHit.Normalise();

    return Ray(viewer.t, zNearHit, std::numeric_limits<float>::max());
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

  std::vector<Matrix> m_modelStack;
  float m_view[Transforms::kNumMatrixElems];
  float m_projection[Transforms::kNumMatrixElems];
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
  Transforms::Updater::SetView(const float matrix[kNumMatrixElems])
{
  s_impl->SetView(matrix);
  return *this;
}

//==============================================================================
Transforms::Updater&
  Transforms::Updater::SetProjection(const float matrix[kNumMatrixElems],
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
void  Transforms::GetView(float matrix[kNumMatrixElems])
{
  s_impl->GetView(matrix);
}

//==============================================================================
void  Transforms::GetProjection(float matrix[kNumMatrixElems])
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
Ray Transforms::GetViewRay(float nx, float ny)
{
  return s_impl->GetViewRay(nx, ny);
}

} // XR

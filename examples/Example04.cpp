//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Example.hpp"
#include "DragController.hpp"
#include "xr/Device.hpp"
#include "xr/Transforms.hpp"
#include "xr/Timer.hpp"
#include "xr/Material.hpp"
#include "xr/Asset.hpp"
#include "xr/Vertex.hpp"
#include "xr/Input.hpp"
#include "xr/math/Quaternion.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

// We are creating 3D geometry, and applying model-view-projections to get it
// onto the screen, using xr::Transforms.
class Example04 : public Example
{
public:
  Example04()
  : Example("04 - Transforms")
  {}

  virtual void Init() override
  {
    Gfx::Init(Device::GetGfxContext());
    Asset::Manager::Init(".assets");

    // Initialise the Transforms subsystem, which gives us projection, view
    // and model matrices to access in shaders.
    Transforms::Init();

    // Create an octahedron. Define vertex data.
    using VertexType = Vertex::Format<Vertex::Pos<>, Vertex::UV0<>, Vertex::Color0<Color>>;
    VertexType vertices[] = {
      VertexType(Vector3::UnitY() * .8f, Vector2(.5f, .5f), Color(1.f, 1.f, 1.f, 0.f)), // CT
      VertexType(Vector3(-1.f, 0.f, 1.f), Vector2(0.f, 0.f), Color(1.f, .5f, .5f, .75f)), // NW0
      VertexType(Vector3(1.f, 0.f, 1.f), Vector2(1.f, 0.f), Color(.5f, 1.f, .5f, .75f)), // NE0
      VertexType(Vector3(1.f, 0.f, -1.f), Vector2(1.f, 1.f), Color(.5f, .5f, 1.f, .75f)),  // SE0
      VertexType(Vector3(-1.f, 0.f, -1.f), Vector2(0.f, 1.f), Color(.5f, .5f, .5f, .75f)), //SW0

      VertexType(Vector3(1.f, 0.f, 1.f), Vector2(1.f, 1.f), Color(.5f, 1.f, .5f, .75f)),  // NE1
      VertexType(Vector3(1.f, 0.f, -1.f), Vector2(1.f, 0.f), Color(.5f, .5f, 1.f, .75f)), // SE1
      VertexType(Vector3(-1.f, 0.f, -1.f), Vector2(0.f, 0.f), Color(.5f, .5f, .5f, .75f)), // SW1
      VertexType(Vector3(-1.f, 0.f, 1.f), Vector2(0.f, 1.f), Color(1.f, .5f, .5f, .75f)),  // NW1
      VertexType(Vector3::UnitY() * -.8f, Vector2(.5f, .5f), Color(0.f, 0.f, 0.f, 0.f)), // CB
    };

    // Register the vertex format to create the vertex buffer with.
    auto vertexFormat = VertexType::Register();
    m_hVertexBuffer = Gfx::CreateVertexBuffer(vertexFormat, Buffer::FromArray(vertices));

    // We can now release our format handle, as the buffer holds a reference to it.
    Gfx::Release(vertexFormat);

    // Define indices.
    std::vector<uint16_t> indices { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1,
      9, 6, 5, 9, 7, 6, 9, 8, 7, 9, 5, 8 };
    m_hIndexBuffer = Gfx::CreateIndexBuffer({ indices.size() * sizeof(decltype(indices)::value_type),
      reinterpret_cast<uint8_t*>(indices.data()) });
    m_numIndices = indices.size();

    // Reset the rotation.
    m_rotation = Quaternion::Identity();

    Texture::RegisterSamplerUniform("uTexture0", 0);
    Gfx::Flush();

    m_loading = true;
    m_material = Asset::Manager::Load<Material>("example04.mtl");

    m_rotation = Quaternion::Identity();
    m_dragControl.Reset();
    m_dragControl.ConnectMouse();
  }

  virtual void Update() override
  {
    if (m_loading)
    {
      Asset::Manager::Update();
      m_loading = !CheckAllMaskBits(m_material->GetFlags(), Asset::ReadyFlag);
    }
    else
    {
      m_dragControl.Update();

      // We apply two rotations: one is slow and automatic, and the other one
      // depends on mouse input. Pressing the mouse button drowns out the
      // automatic rotation.
      float weight = 1.f - m_dragControl.GetPressTimerPercent();

      const float controlPoints[] = { 0.f, .12f, .88f, 1.f };
      weight = Bezier(controlPoints, weight);

      auto motion = m_dragControl.GetFrameMotion() * 0.02f;
      m_rotation *= Quaternion::FromAxisAngle(Vector3(.2, .5, 0.f), weight * .02f) *
        Quaternion::FromPitchYawRoll(motion.x, motion.y, .0);
    }
  }

  virtual void Render() override
  {
    if (m_loading)
    {
      Gfx::Clear(Gfx::F_CLEAR_COLOR | Gfx::F_CLEAR_DEPTH, Color(FlopRand(), FlopRand(), FlopRand()));
      return;
    }

    Gfx::Clear(Gfx::F_CLEAR_COLOR | Gfx::F_CLEAR_DEPTH, Color(0.f, .25f, .5f));

    m_material->Apply();

    // Position the mesh in 3D, using the rotation and an offset from camera.
    XR_TRANSFORMS_SCOPED_MODEL(Matrix(m_rotation, Vector3::UnitZ() * -5.f));

    // Render mesh as list of triangles. We're rendering it twice for a 3d transparency
    // effect, culling front faces first, then back faces.
    Gfx::SetState(m_material->GetStateFlags() | Gfx::F_STATE_CULL_FRONT);
    Gfx::Draw(m_hVertexBuffer, m_hIndexBuffer, Primitive::TriList, 0, m_numIndices);

    Gfx::SetState(m_material->GetStateFlags() | Gfx::F_STATE_CULL_BACK);
    Gfx::Draw(m_hVertexBuffer, m_hIndexBuffer, Primitive::TriList, 0, m_numIndices);

    Gfx::Present();
  }

  virtual void Shutdown() override
  {
    m_material.Reset(nullptr);

    // Release buffers used for the mesh.
    Gfx::Release(m_hVertexBuffer);
    m_hVertexBuffer.Invalidate();

    Gfx::Release(m_hIndexBuffer);
    m_hIndexBuffer.Invalidate();

    Asset::Manager::Shutdown();
    Gfx::Shutdown();
  }

private:
  Gfx::VertexBufferHandle m_hVertexBuffer;
  Gfx::IndexBufferHandle m_hIndexBuffer;
  uint32_t m_numIndices;

  Quaternion m_rotation;

  bool m_loading;
  Material::Ptr  m_material;

  DragController m_dragControl;
  Vector2 m_motion;
} example;

}

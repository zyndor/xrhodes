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
#include "xr/IndexMesh.hpp"
#include "xr/Vertex.hpp"
#include "xr/math/Quaternion.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

// We're simplifying vertex / index data management by using IndexMesh, then
// render several instances of the same geometry with a single draw call, using
// an instance data buffer.
class Example06 : public Example
{
public:
  Example06()
  : Example("06 - Instanced Rendering")
  {}

  virtual void Init() override
  {
    Gfx::Init(Device::GetGfxContext());
    Asset::Manager::Init(".assets");
    Transforms::Init();

    using VertexType = Vertex::Format<Vertex::Pos<>, Vertex::UV0<>, Vertex::Color0<Color>>;
    VertexType vertices[] = {
      VertexType(Vector3::UnitY() * .8f, Vector2(.5f, .5f), Color(1.f, 1.f, 1.f, 0.f)), // CT
      VertexType(Vector3(-1.f, 0.f, 1.f), Vector2(0.f, 0.f), Color(1.f, .5f, .5f)), // SW0
      VertexType(Vector3(1.f, 0.f, 1.f), Vector2(1.f, 0.f), Color(.5f, 1.f, .5f)), // SE0
      VertexType(Vector3(1.f, 0.f, -1.f), Vector2(1.f, 1.f), Color(.5f, .5f, 1.f)),  // NE0
      VertexType(Vector3(-1.f, 0.f, -1.f), Vector2(0.f, 1.f), Color(.5f, .5f, .5f)), //NW0

      VertexType(Vector3(1.f, 0.f, 1.f), Vector2(1.f, 1.f), Color(.5f, 1.f, .5f)),  // SE1
      VertexType(Vector3(1.f, 0.f, -1.f), Vector2(1.f, 0.f), Color(.5f, .5f, 1.f)), // NE1
      VertexType(Vector3(-1.f, 0.f, -1.f), Vector2(0.f, 0.f), Color(.5f, .5f, .5f)), // NW1
      VertexType(Vector3(-1.f, 0.f, 1.f), Vector2(0.f, 1.f), Color(1.f, .5f, .5f)),  // SW1
      VertexType(Vector3::UnitY() * -.8f, Vector2(.5f, .5f), Color(0.f, 0.f, 0.f, 0.f)), // CB
    };

    std::vector<uint16_t> indices { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1,
      9, 8, 7, 9, 7, 6, 9, 6, 5, 9, 5, 8 };

    // Create IndexMesh
    m_mesh = IndexMesh::Create(XR_ARRAY_SIZE(vertices), vertices, indices.size(), indices.data());

    // Create a large number of instances and lay them out on the surface of a sphere.
    m_numInstances = 2048;
    struct InstanceData
    {
      Vector3 pos;
      float scale;
      Quaternion rotation;
    };
    std::vector<InstanceData> instanceData;
    instanceData.reserve(m_numInstances);

    float inc = kPi * (3.f - std::sqrt(5.f));
    float off = 2.f / instanceData.capacity();
    InstanceData id;
    for (size_t i = 0; i < m_numInstances; ++i)
    {
      id.pos.y = i * off - 1.f + off * .5f;
      float r = std::sqrt(1.f - id.pos.y * id.pos.y);
      float phi = i * inc;
      id.pos.x = std::cos(phi) * r;
      id.pos.z = std::sin(phi) * r;
      id.pos.Normalise();

      id.rotation = Quaternion::FromPositions(Vector3::UnitY(), id.pos);
      id.pos *= 25.f;
      id.scale = ((i % 3) + 1) / 3.f;

      instanceData.push_back(id);
    }

    // Upload instance data.
    m_hInstanceData = Gfx::CreateInstanceDataBuffer({ instanceData.size() * sizeof(InstanceData), reinterpret_cast<uint8_t*>(instanceData.data()) }, sizeof(InstanceData));

    Texture::RegisterSamplerUniform("uTexture0", 0);
    Gfx::Flush();

    m_loading = true;
    m_material = Asset::Manager::Load<Material>("example06.mtl");

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

      float weight = 1.f - m_dragControl.GetPressTimerPercent();

      const float controlPoints[] = { 0.f, .12f, .88f, 1.f };
      weight = Bezier<float, 4>(controlPoints, weight);

      auto motion = m_dragControl.GetFrameMotion() * 0.005f;
      m_rotation *= Quaternion::FromAxisAngle(Vector3(.2, .5, 0.f), weight * .002f) *
        Quaternion::FromPitchYawRoll(motion.x, motion.y, .0);
    }
  }

  virtual void Render() override
  {
    if (m_loading)
    {
      Gfx::Clear(Gfx::F_CLEAR_COLOR | Gfx::F_CLEAR_DEPTH, Color(FlopRand(), FlopRand(), FlopRand()));
      Gfx::Present();
      return;
    }

    Gfx::Clear(Gfx::F_CLEAR_COLOR | Gfx::F_CLEAR_DEPTH, Color(0.f, .25f, .5f));

    m_material->Apply();

    XR_TRANSFORMS_SCOPED_MODEL(Matrix(m_rotation, Vector3::UnitZ() * -50.f));

    // Set instance data
    Gfx::SetInstanceData(m_hInstanceData, 0, m_numInstances);

    // Render mesh (instances -- since we've set instance data).
    m_mesh.Render(Primitive::TriList);

    Gfx::Present();
  }

  virtual void Shutdown() override
  {
    m_dragControl.DisconnectMouse();

    m_mesh = IndexMesh();

    Gfx::Release(m_hInstanceData);
    m_hInstanceData.Invalidate();

    m_material.Reset(nullptr);

    Asset::Manager::Shutdown();
    Gfx::Shutdown();
  }

private:
  IndexMesh m_mesh;
  Gfx::InstanceDataBufferHandle m_hInstanceData;
  uint32_t m_numInstances;

  bool m_loading;
  Material::Ptr  m_material;

  DragController m_dragControl;
  Quaternion m_rotation;
} example;

}

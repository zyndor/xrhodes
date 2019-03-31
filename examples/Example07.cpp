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
#include "xr/meshutil.hpp"
#include "xr/Mesh.hpp"
#include "xr/Vertex.hpp"
#include "xr/math/Vector4.hpp"
#include "xr/math/Quaternion.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

// We're adding a directional light to the previous example, for which we need to
// add normals to our vertex format, and update the shaders.
class Example07 : public Example
{
public:
  Example07()
  : Example("07 - Phong Shading")
  {}

  virtual void Init() override
  {
    Gfx::Init(Device::GetGfxContext());
    Asset::Manager::Init(".assets");
    Transforms::Init();

    // Add normals to vertex format.
    using VertexType = Vertex::Format<Vertex::Pos<>, Vertex::UV0<>, Vertex::Color0<Vector3>, Vertex::Normal>;

    // While we won't specify normals manually, we'll need to replace shared
    // vertices from the last example, with unique ones. This means that we
    // no longer need an index buffer, and can use Mesh instead of IndexMesh.
    VertexType vertices[] = {
      VertexType(Vector3::UnitY() * .8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)), // CT
      VertexType(Vector3(-1.f, 0.f, 1.f), Vector2(0.f, 0.f), Vector3(1.f, .5f, .5f)), // NW0
      VertexType(Vector3(1.f, 0.f, 1.f), Vector2(1.f, 0.f), Vector3(.5f, 1.f, .5f)), // NE0

      VertexType(Vector3::UnitY() * .8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)), // CT
      VertexType(Vector3(1.f, 0.f, 1.f), Vector2(1.f, 0.f), Vector3(.5f, 1.f, .5f)), // NE1
      VertexType(Vector3(1.f, 0.f, -1.f), Vector2(1.f, 1.f), Vector3(.5f, .5f, 1.f)),  // SE0

      VertexType(Vector3::UnitY() * .8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)), // CT
      VertexType(Vector3(1.f, 0.f, -1.f), Vector2(1.f, 1.f), Vector3(.5f, .5f, 1.f)),  // SE1
      VertexType(Vector3(-1.f, 0.f, -1.f), Vector2(0.f, 1.f), Vector3(.5f, .5f, .5f)), //SW0

      VertexType(Vector3::UnitY() * .8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)), // CT
      VertexType(Vector3(-1.f, 0.f, -1.f), Vector2(0.f, 1.f), Vector3(.5f, .5f, .5f)), //SW1
      VertexType(Vector3(-1.f, 0.f, 1.f), Vector2(0.f, 0.f), Vector3(1.f, .5f, .5f)), // NW1

      VertexType(Vector3::UnitY() * -.8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)), // CT
      VertexType(Vector3(1.f, 0.f, 1.f), Vector2(1.f, 1.f), Vector3(.5f, 1.f, .5f)), // NE0
      VertexType(Vector3(-1.f, 0.f, 1.f), Vector2(0.f, 1.f), Vector3(1.f, .5f, .5f)), // NW0

      VertexType(Vector3::UnitY() * -.8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)), // CT
      VertexType(Vector3(1.f, 0.f, -1.f), Vector2(1.f, 0.f), Vector3(.5f, .5f, 1.f)),  // SE0
      VertexType(Vector3(1.f, 0.f, 1.f), Vector2(1.f, 1.f), Vector3(.5f, 1.f, .5f)), // NE1

      VertexType(Vector3::UnitY() * -.8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)), // CT
      VertexType(Vector3(-1.f, 0.f, -1.f), Vector2(0.f, 0.f), Vector3(.5f, .5f, .5f)), //SW0
      VertexType(Vector3(1.f, 0.f, -1.f), Vector2(1.f, 0.f), Vector3(.5f, .5f, 1.f)),  // SE1

      VertexType(Vector3::UnitY() * -.8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)), // CT
      VertexType(Vector3(-1.f, 0.f, 1.f), Vector2(0.f, 1.f), Vector3(1.f, .5f, .5f)), // NW1
      VertexType(Vector3(-1.f, 0.f, -1.f), Vector2(0.f, 0.f), Vector3(.5f, .5f, .5f)), //SW1
    };

    // Get meshutil to calculate normals for our triangles.
    meshutil::CalculateTrianglesNormals(XR_ARRAY_SIZE(vertices), vertices);

    // Create mesh
    m_mesh = Mesh::Create(XR_ARRAY_SIZE(vertices), vertices);

    m_numInstances = 32;
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
      id.scale = ((i % 3) + 1) / 3.f;
      id.pos *= 4.5f * id.scale;

      instanceData.push_back(id);
    }

    // Upload instance data.
    m_hInstanceData = Gfx::CreateInstanceDataBuffer({ instanceData.size() * sizeof(InstanceData), reinterpret_cast<uint8_t*>(instanceData.data()) }, sizeof(InstanceData));

    // Create uniforms for lighting
    m_uLightColor = Gfx::CreateUniform("uLightColor", Gfx::UniformType::Vec4);
    m_uLightDirection = Gfx::CreateUniform("uLightDir", Gfx::UniformType::Vec4);

    Vector4 lightDirection = Vector4::From(-Vector3::UnitY());
    Gfx::SetUniform(m_uLightDirection, &lightDirection);

    Texture::RegisterSamplerUniform("uTexture0", 0);
    Gfx::Flush();

    m_rotation = Quaternion::Identity();
    m_dragControl.Reset();
    m_dragControl.ConnectMouse();

    m_loading = true;
    m_material = Asset::Manager::Load<Material>("example07.mtl");
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
      weight = Bezier(controlPoints, weight);

      auto motion = m_dragControl.GetFrameMotion() * 0.01f;
      m_rotation *= Quaternion::FromAxisAngle(Vector3(.2, .5, 0.f), weight * .002f) *
        Quaternion::FromPitchYawRoll(motion.x, motion.y, 0.);

      // Change the color of the light over time.
      Color lightColor(.75f, .66f + sinf(Timer::GetUST() * .0002f) * .333, .5f);
      Gfx::SetUniform(m_uLightColor, &lightColor);
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

    Gfx::Clear(Gfx::F_CLEAR_COLOR | Gfx::F_CLEAR_DEPTH, Color(0.f, .125f, .25f));

    m_material->Apply();

    XR_TRANSFORMS_SCOPED_MODEL(Matrix(m_rotation, Vector3::UnitZ() * -10.f));

    // Set instance data
    Gfx::SetInstanceData(m_hInstanceData, 0, m_numInstances);

    // Render mesh (instances -- since we've set instance data).
    m_mesh.Render(Primitive::TriangleList);

    Gfx::Present();
  }

  virtual void Shutdown() override
  {
    m_dragControl.DisconnectMouse();

    m_material.Reset(nullptr);

    m_mesh = Mesh();

    Gfx::Release(m_hInstanceData);
    m_hInstanceData.Invalidate();

    Gfx::Release(m_uLightColor);
    m_uLightColor.Invalidate();

    Gfx::Release(m_uLightDirection);
    m_uLightDirection.Invalidate();

    Asset::Manager::Shutdown();
    Gfx::Shutdown();
  }

private:
  Mesh m_mesh;
  Gfx::InstanceDataBufferHandle m_hInstanceData;
  uint32_t m_numInstances;

  Gfx::UniformHandle m_uLightDirection;
  Gfx::UniformHandle m_uLightColor;

  bool m_loading;
  Material::Ptr  m_material;

  DragController m_dragControl;
  Quaternion m_rotation;
} example;

}

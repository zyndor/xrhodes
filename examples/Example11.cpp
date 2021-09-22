//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Example.hpp"
#include "AssetQueue.hpp"
#include "DragController.hpp"
#include "xr/Texture.hpp"
#include "xr/Device.hpp"
#include "xr/Transforms.hpp"
#include "xr/Material.hpp"
#include "xr/meshutil.hpp"
#include "xr/IndexMesh.hpp"
#include "xr/Vertex.hpp"
#include "xr/math/Vector4.hpp"
#include "xr/math/Quaternion.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

class Example11 : public Example
{
public:
  Example11()
  : Example("11 - Image Based Lighting")
  {}

  virtual void Init() override
  {
    Gfx::Init(Device::GetGfxContext());
    Asset::Manager::Init(".assets");
    Transforms::Init();

    // Create uniforms required for IBL - maximum level of detail and rotation of environment.
    m_uMaxLod = Gfx::CreateUniform("uMaxLod", Gfx::UniformType::Vec4);
    Vector4 maxLod(6.f);
    Gfx::SetUniform(m_uMaxLod, maxLod.begin());

    m_uEnvRotation = Gfx::CreateUniform("uEnvRotation", Gfx::UniformType::Vec4);

    using VertexType = Vertex::Format<Vertex::Pos<>>;

    VertexType vertices[8] = {
      Vector3(-1.f, -1.f, -1.f),
      Vector3(1.f, -1.f, -1.f),

      Vector3(-1.f, -1.f, 1.f),
      Vector3(1.f, -1.f, 1.f),

      Vector3(-1.f, 1.f, -1.f),
      Vector3(1.f, 1.f, -1.f),

      Vector3(-1.f, 1.f, 1.f),
      Vector3(1.f, 1.f, 1.f),
    };

    //    4----5
    //   /|   /|
    //  6----7 |
    //  | |  | |
    //  | 0--|-1
    //  |/   |/
    //  2----3
    std::vector<uint16_t> indices {
      4, 0, 5,
      0, 1, 5,

      6, 2, 4,
      2, 0, 4,

      7, 3, 6,
      3, 2, 6,

      5, 1, 7,
      1, 3, 7,

      6, 4, 7,
      4, 5, 7,

      0, 2, 1,
      2, 3, 1,
    };

    m_skyboxMesh = IndexMesh::Create(XR_ARRAY_SIZE(vertices), vertices, indices.size(),
      indices.data());

    Texture::RegisterSamplerUniform("uTexture0", 0);

    // Register additional samplers for IBL.
    Texture::RegisterSamplerUniform("uIrradiance", 1);
    Texture::RegisterSamplerUniform("uRadiance", 2);

    m_skyboxShader = Asset::Manager::Load<Shader>("skybox.shd");
    m_assetQueue.Add(m_skyboxShader);

    m_cubemap = Asset::Manager::Load<Texture>("skybox.tex");
    m_assetQueue.Add(m_cubemap);

    m_objectsMaterial = Asset::Manager::Load<Material>("example11.mtl");
    m_assetQueue.Add(m_objectsMaterial);

    CreateObjects();

    Transforms::Updater().SetViewerTransform(Matrix(Vector3::UnitZ() * 5.0f));

    m_rotation = Quaternion::Identity();
    m_dragControl.Reset();
    m_dragControl.ConnectMouse();
  }

  virtual void Update() override
  {
    if (!m_assetQueue.IsReady())
    {
      m_assetQueue.Update();
    }
    else
    {
      m_dragControl.Update();

      float weight = 1.f - m_dragControl.GetPressTimerPercent();

      const float controlPoints[] = { 0.f, .12f, .88f, 1.f };
      weight = Bezier(controlPoints, weight);

      auto motion = m_dragControl.GetFrameMotion() * 0.01f;
      m_rotation *= Quaternion::FromPitchYawRoll(motion.y, motion.x, 0.);
    }
  }

  virtual void Render() override
  {
    if (!m_assetQueue.IsReady())
    {
      Gfx::Clear(Gfx::F_CLEAR_COLOR | Gfx::F_CLEAR_DEPTH, Color(FlopRand(), FlopRand(), FlopRand()));
      Gfx::Present();
      return;
    }

    Gfx::Clear(Gfx::F_CLEAR_DEPTH);

    // Calculate environment rotation and update IBL uniform.
    auto q = Quaternion::FromAxisAngle(Vector3::UnitY(), kPi);
    auto rotation = m_rotation;
    rotation.Conjugate();
    rotation *= q;
    Gfx::SetUniform(m_uEnvRotation, rotation.begin());

    {
      XR_TRANSFORMS_SCOPED_MODEL(Matrix(m_rotation, Vector3::UnitZ() * -20.f));

      m_objectsMaterial->Apply();
      Gfx::SetInstanceData(m_hInstanceData, 0, m_numInstances);
      m_objectMesh.Render(Primitive::TriangleList);
    }

    // draw skybox
    {
      Matrix model(m_rotation);
      model.ScaleLinear(5.f);
      XR_TRANSFORMS_SCOPED_MODEL(model);

      Gfx::SetState(Gfx::F_STATE_CULL_BACK | Gfx::F_STATE_DEPTH_TEST);
      m_cubemap->Bind(0);
      m_skyboxShader->Use();
      m_skyboxMesh.Render(Primitive::TriangleList);
    }

    Gfx::Flush();

    Gfx::Present();
  }

  virtual void Shutdown() override
  {
    m_dragControl.DisconnectMouse();

    m_objectMesh = IndexMesh();

    Gfx::Release(m_hInstanceData);
    m_hInstanceData.Invalidate();

    m_objectsMaterial.Reset(nullptr);

    m_skyboxMesh = IndexMesh();

    m_cubemap.Reset(nullptr);
    m_skyboxShader.Reset(nullptr);

    Gfx::Release(m_uMaxLod);
    Gfx::Release(m_uEnvRotation);

    Asset::Manager::Shutdown();
    Gfx::Shutdown();
  }

private:
  Gfx::UniformHandle  m_uEnvRotation;
  Gfx::UniformHandle  m_uMaxLod;

  AssetQueue m_assetQueue;

  DragController m_dragControl;
  Quaternion m_rotation;

  Shader::Ptr m_skyboxShader;
  Texture::Ptr m_cubemap;
  IndexMesh m_skyboxMesh;

  Material::Ptr m_objectsMaterial;
  IndexMesh m_objectMesh;
  uint32_t m_numInstances;
  Gfx::InstanceDataBufferHandle m_hInstanceData;

  void CreateObjects()
  {
    // N.B. that there are no normals and tangents in the vertex format --
    // we're going to calculate them on the fly.
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

    // We're just going to rotate the mesh data so the instances face us as
    // the rotation has been removed from the instance data for this example.
    for (auto& v : vertices)
    {
      v.pos = Quaternion::FromAxisAngle(Vector3::UnitX(), kPi * .5f).Rotate(v.pos);
    }

    std::vector<uint16_t> indices{ 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1,
      9, 8, 7, 9, 7, 6, 9, 6, 5, 9, 5, 8 };

    m_objectMesh = IndexMesh::Create(XR_ARRAY_SIZE(vertices), vertices, indices.size(), indices.data());

    // Create 5 x 5 grid of objects with roughness and metalness going from
    // 0 to 1 along the x and y axes, respectively.
    const decltype(m_numInstances) instances = 5;
    m_numInstances = instances * instances;
    struct InstanceData
    {
      Vector3 pos;
      float scale;
      Vector4 metalnessRoughness;
    };
    std::vector<InstanceData> instanceData;
    instanceData.reserve(m_numInstances);

    float spacing = 4.f;
    float offset = spacing * (instances - 1) * -.5f;
    InstanceData id;
    id.scale = 1.5f;
    id.pos = { offset, -offset, 0.f };
    for (int y = 0; y < instances; ++y)
    {
      id.metalnessRoughness.x = y / float(instances - 1);
      for (int x = 0; x < instances; ++x)
      {
        id.metalnessRoughness.y = x / float(instances - 1);
        instanceData.push_back(id);
        id.pos.x += spacing;
      }
      id.pos.x = offset;
      id.pos.y -= spacing;
    }

    m_hInstanceData = Gfx::CreateInstanceDataBuffer({ instanceData.size() * sizeof(InstanceData), reinterpret_cast<uint8_t*>(instanceData.data()) }, sizeof(InstanceData));
  }
} example;

}

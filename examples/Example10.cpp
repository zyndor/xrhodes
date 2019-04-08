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

/*
The skybox textures are the work of Emil Persson, aka Humus.
http://www.humus.name

This work is licensed under a Creative Commons Attribution 3.0 Unported License.
http://creativecommons.org/licenses/by/3.0/
*/
const char* kCubeMapTextures[] = {
  "posx",
  "negx",
  "posy",
  "negy",
  "posz",
  "negz"
};

class CubemapExample1 : public Example
{
public:
  CubemapExample1()
  : Example("10 - Cubemap")
  {}

  virtual void Init() override
  {
    Gfx::Init(Device::GetGfxContext());
    Asset::Manager::Init(".assets");
    Transforms::Init();

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

    Texture::RegisterSamplerUniform("uTextureCube", 0);

    m_skyboxShader = Asset::Manager::Load<Shader>("skybox.shd");
    m_assetQueue.Add(m_skyboxShader);

    m_cubemap = Asset::Manager::Load<Texture>("skybox.tex");
    m_assetQueue.Add(m_cubemap);

    m_objectsMaterial = Asset::Manager::Load<Material>("example06.mtl");
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
      m_rotation *= Quaternion::FromPitchYawRoll(motion.x, motion.y, 0.);
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

    // Clear depth buffer only - the skybox will fill the screen anyway.
    Gfx::Clear(Gfx::F_CLEAR_DEPTH);

    // draw objects
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

    Asset::Manager::Shutdown();
    Gfx::Shutdown();
  }

private:
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

    std::vector<uint16_t> indices{ 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1,
      9, 8, 7, 9, 7, 6, 9, 6, 5, 9, 5, 8 };

    m_objectMesh = IndexMesh::Create(XR_ARRAY_SIZE(vertices), vertices, indices.size(), indices.data());

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
      id.pos *= 8.f;
      id.scale = 1.f;

      instanceData.push_back(id);
    }

    m_hInstanceData = Gfx::CreateInstanceDataBuffer({ instanceData.size() * sizeof(InstanceData), reinterpret_cast<uint8_t*>(instanceData.data()) }, sizeof(InstanceData));
  }
} example;

}

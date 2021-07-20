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
#include "xr/ScratchBuffer.hpp"
#include "xr/Device.hpp"
#include "xr/Transforms.hpp"
#include "xr/Timer.hpp"
#include "xr/Material.hpp"
#include "xr/Asset.hpp"
#include "xr/meshutil.hpp"
#include "xr/Mesh.hpp"
#include "xr/Vertex.hpp"
#include "xr/ProjectionHelper.hpp"
#include "xr/math/Vector4.hpp"
#include "xr/math/Quaternion.hpp"
#include "xr/math/Matrix4.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

// We're demonstrating off-screen rendering by adding shadowmaps to our lighting
// model from the previous example.

// Let's create a viewer with a position / orientation in the world, and a projection.
struct Viewer
{
  Matrix worldTransform;
  Matrix4 projection;
};

// Bundle up all information required for a render pass into a structure.
struct RenderPass
{
  enum
  {
    DEPTH,
    MAIN
  };

  Gfx::FrameBufferHandle  hFbo;
  Viewer viewer;
  Rect viewport;
  Gfx::FlagType clearBits;
  float clearDepth;
  Color clearColor;
  Material::Ptr material;
};

class Example08 : public Example
{
public:
  Example08()
  : Example("08 - Shadowmaps")
  {}

  virtual void Init() override
  {
    Gfx::Init(Device::GetGfxContext());
    Asset::Manager::Init(".assets");
    Transforms::Init();

    ScratchBuffer::Init(128);

    using VertexType = Vertex::Format<Vertex::Pos<>, Vertex::UV0<>, Vertex::Color0<Vector3>, Vertex::Normal>;

    VertexType vertices[] = {
      VertexType(Vector3::UnitY() * .8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)),
      VertexType(Vector3(-1.f, 0.f, 1.f), Vector2(0.f, 0.f), Vector3(1.f, .5f, .5f)),
      VertexType(Vector3(1.f, 0.f, 1.f), Vector2(1.f, 0.f), Vector3(.5f, 1.f, .5f)),

      VertexType(Vector3::UnitY() * .8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)),
      VertexType(Vector3(1.f, 0.f, 1.f), Vector2(1.f, 0.f), Vector3(.5f, 1.f, .5f)),
      VertexType(Vector3(1.f, 0.f, -1.f), Vector2(1.f, 1.f), Vector3(.5f, .5f, 1.f)),

      VertexType(Vector3::UnitY() * .8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)),
      VertexType(Vector3(1.f, 0.f, -1.f), Vector2(1.f, 1.f), Vector3(.5f, .5f, 1.f)),
      VertexType(Vector3(-1.f, 0.f, -1.f), Vector2(0.f, 1.f), Vector3(.5f, .5f, .5f)),

      VertexType(Vector3::UnitY() * .8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)),
      VertexType(Vector3(-1.f, 0.f, -1.f), Vector2(0.f, 1.f), Vector3(.5f, .5f, .5f)),
      VertexType(Vector3(-1.f, 0.f, 1.f), Vector2(0.f, 0.f), Vector3(1.f, .5f, .5f)),

      VertexType(Vector3::UnitY() * -.8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)),
      VertexType(Vector3(1.f, 0.f, 1.f), Vector2(1.f, 1.f), Vector3(.5f, 1.f, .5f)),
      VertexType(Vector3(-1.f, 0.f, 1.f), Vector2(0.f, 1.f), Vector3(1.f, .5f, .5f)),

      VertexType(Vector3::UnitY() * -.8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)),
      VertexType(Vector3(1.f, 0.f, -1.f), Vector2(1.f, 0.f), Vector3(.5f, .5f, 1.f)),
      VertexType(Vector3(1.f, 0.f, 1.f), Vector2(1.f, 1.f), Vector3(.5f, 1.f, .5f)),

      VertexType(Vector3::UnitY() * -.8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)),
      VertexType(Vector3(-1.f, 0.f, -1.f), Vector2(0.f, 0.f), Vector3(.5f, .5f, .5f)),
      VertexType(Vector3(1.f, 0.f, -1.f), Vector2(1.f, 0.f), Vector3(.5f, .5f, 1.f)),

      VertexType(Vector3::UnitY() * -.8f, Vector2(.5f, .5f), Vector3(1.f, 1.f, 1.f)),
      VertexType(Vector3(-1.f, 0.f, 1.f), Vector2(0.f, 1.f), Vector3(1.f, .5f, .5f)),
      VertexType(Vector3(-1.f, 0.f, -1.f), Vector2(0.f, 0.f), Vector3(.5f, .5f, .5f)),
    };

    meshutil::CalculateTrianglesNormals(XR_ARRAY_SIZE(vertices), vertices);

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

    m_hInstanceData = Gfx::CreateInstanceDataBuffer({ instanceData.size() * sizeof(InstanceData), reinterpret_cast<uint8_t*>(instanceData.data()) }, sizeof(InstanceData));

    m_uLightColor = Gfx::CreateUniform("uLightColor", Gfx::UniformType::Vec4);

    // Set up uniforms required for shadow projection.
    // We're packing the inverse shadow map size (assuming a square texture) with the light direction.
    m_uLightDirection_InvShadowMapSize = Gfx::CreateUniform("uLightDir_InvShadowMapSize", Gfx::UniformType::Vec4);
    m_uLightSpaceTransform =
      Gfx::CreateUniform("uLightSpaceTransform", Gfx::UniformType::Mat4);

    Texture::RegisterSamplerUniform("uTexture0", 0);

    // Register sampler for shadow map and set texture.
    // NOTE: in this example, there's always the same amount of textures bound,
    // hence we can get away only doing this once.
    Texture::RegisterSamplerUniform("uShadowMap", 1);

    // Create render target for shadow pass. We only need a depth buffer.
    Px  shadowMapSize = 1024;
    auto hShadowMap = Gfx::CreateTexture(Gfx::TextureFormat::D32,
        shadowMapSize, shadowMapSize, 0, Gfx::F_TEXTURE_FILTER_POINT);

    // Set up renderpasses
    float orthoSize = 11.5f;
    m_renderPasses[RenderPass::DEPTH] = { // depth pre-pass
      Gfx::CreateFrameBuffer(1, &hShadowMap),
      {
        Matrix{ Quaternion::FromAxisAngle(Vector3::UnitX(), kPiHalf),
          Vector3(0., orthoSize, -10.) },
      },
      Rect{ 0, 0, shadowMapSize, shadowMapSize },
      Gfx::F_CLEAR_DEPTH,
      1.,
      Color{},  // no color target; ignored
      Asset::Manager::Load<Material>("example08-depth.mtl")
    };
    ProjectionHelper::CalculateOrthographic(orthoSize, 0.f, orthoSize * 2.f,
      m_renderPasses[RenderPass::DEPTH].viewer.projection.data);
    m_assetQueue.Add(m_renderPasses[RenderPass::DEPTH].material);

    m_renderPasses[RenderPass::MAIN] = { // main pass
      Gfx::GetDefaultFrameBuffer(),
      {
        Matrix::Identity(),
      },
      Rect{ 0, 0, Gfx::GetLogicalWidth(), Gfx::GetLogicalHeight() },
      Gfx::F_CLEAR_DEPTH | Gfx::F_CLEAR_COLOR,
      1.,
      Color(0., 0.125, 0.25),
      Asset::Manager::Load<Material>("example08.mtl")
    };
    ProjectionHelper::CalculatePerspective(kPi * .25f,
      Gfx::GetLogicalAspectRatio(), 0.1, 25.,
      m_renderPasses[RenderPass::MAIN].viewer.projection.data);
    m_assetQueue.Add(m_renderPasses[RenderPass::MAIN].material);

    // Transfer ownership of shadowmap to managed pointer.
    m_shadowMap = Texture::FromHandle(hShadowMap);
    Gfx::Flush();

    // Load shader for visualising depth buffer
    m_debugShader = Asset::Manager::Load<Shader>("example08-debug.shd");
    m_assetQueue.Add(m_debugShader);

    m_rotation = Quaternion::Identity();
    m_dragControl.Reset();
    m_dragControl.ConnectMouse();
  }

  virtual void Update() override
  {
    if (!m_assetQueue.IsReady())
    {
      m_assetQueue.Update();

      if (m_assetQueue.IsReady())
      {
        m_renderPasses[RenderPass::MAIN].material->SetTexture(1, m_shadowMap);
      }
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
      Color lightColor(.75f, .66f + sinf(float(Timer::GetUST() * .0002)) * .333f, .5f);
      Gfx::SetUniform(m_uLightColor, &lightColor);

      // Update the light space transform
      auto const& viewer = m_renderPasses[RenderPass::DEPTH].viewer;
      Matrix lightWorld(viewer.worldTransform);
      lightWorld.Invert();
      lightWorld.t = lightWorld.RotateVec(-lightWorld.t);

      Matrix4 lightView;
      lightView.Import(lightWorld);

      Matrix4 lightWorldProjection;
      lightView.Transform(viewer.projection, lightWorldProjection);

      lightView.SetIdentity(.5);
      lightView.SetTranslation(Vector3::One() * .5);
      lightView.Transform(lightWorldProjection);

      Gfx::SetUniform(m_uLightSpaceTransform, lightWorldProjection.data);

      auto lightDirInvShadowMapSize = Vector4::From(viewer.worldTransform.GetColumn(Vector3::Z),
        1.f / m_shadowMap->GetWidth());
      Gfx::SetUniform(m_uLightDirection_InvShadowMapSize, lightDirInvShadowMapSize.begin());
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

    for (auto& rp: m_renderPasses)
    {
      Gfx::SetFrameBuffer(rp.hFbo);
      Gfx::SetViewport(rp.viewport);
      Transforms::Updater().SetProjection(rp.viewer.projection).
        SetViewerTransform(rp.viewer.worldTransform);
      Gfx::Clear(rp.clearBits, rp.clearColor, rp.clearDepth);

      rp.material->Apply();

      XR_TRANSFORMS_SCOPED_MODEL(Matrix(m_rotation, Vector3::UnitZ() * -10.f));

      // Set instance data
      Gfx::SetInstanceData(m_hInstanceData, 0, m_numInstances);

      // Render mesh (instances -- since we've set instance data).
      m_mesh.Render(Primitive::TriangleList);

      Gfx::Flush();
    }

    // Visualise the contents of the depth buffer - draw a textured rect in
    // the bottom left corner of the screen.
    Gfx::SetViewport(m_renderPasses[RenderPass::MAIN].viewport);
    Gfx::Clear(Gfx::F_CLEAR_DEPTH);
    Transforms::Updater().
      SetOrthographicProjection(-100.f * Gfx::GetLogicalAspectRatio(), 100.f * Gfx::GetLogicalAspectRatio(), -100.f, 100.f, -100.f, 100.f).
      SetViewerTransform(Matrix::Identity());

    XR_TRANSFORMS_SCOPED_MODEL(Matrix(Vector3(-80., -60., .0)));
    m_shadowMap->Bind(0);
    Gfx::SetState(Gfx::F_STATE_NONE);
    m_debugShader->Use();

    auto  verts = ScratchBuffer::Start2dUv(4);
    const float quadSize = 25.f;
    verts[0].pos = Vector2(-quadSize, -quadSize);
    verts[0].uv0 = Vector2(0., 0.);
    verts[1].pos = Vector2(quadSize, -quadSize);
    verts[1].uv0 = Vector2(1., 0.);
    verts[2].pos = Vector2(-quadSize, quadSize);
    verts[2].uv0 = Vector2(0., 1.);
    verts[3].pos = Vector2(quadSize, quadSize);
    verts[3].uv0 = Vector2(1., 1.);
    ScratchBuffer::Finish(Primitive::TriangleStrip);

    Gfx::Present();
  }

  virtual void Shutdown() override
  {
    m_dragControl.DisconnectMouse();

    for (auto& rp: m_renderPasses)
    {
      rp.material.Reset(nullptr);
    }

    m_shadowMap.Reset(nullptr);
    m_debugShader.Reset(nullptr);

    m_mesh = Mesh();

    Gfx::Release(m_hInstanceData);
    m_hInstanceData.Invalidate();

    Gfx::Release(m_renderPasses[RenderPass::DEPTH].hFbo);
    m_renderPasses[RenderPass::DEPTH].hFbo.Invalidate();

    Gfx::Release(m_uLightSpaceTransform);
    m_uLightSpaceTransform.Invalidate();

    Gfx::Release(m_uLightColor);
    m_uLightColor.Invalidate();

    Gfx::Release(m_uLightDirection_InvShadowMapSize);
    m_uLightDirection_InvShadowMapSize.Invalidate();

    Asset::Manager::Shutdown();
    Gfx::Shutdown();
  }

private:
  Mesh m_mesh;
  Gfx::InstanceDataBufferHandle m_hInstanceData;
  uint32_t m_numInstances;

  Gfx::UniformHandle m_uLightSpaceTransform;
  Gfx::UniformHandle m_uLightDirection_InvShadowMapSize;
  Gfx::UniformHandle m_uLightColor;

  AssetQueue m_assetQueue;

  DragController m_dragControl;
  Quaternion m_rotation;

  Texture::Ptr m_shadowMap;
  Shader::Ptr m_debugShader;

  RenderPass  m_renderPasses[2];
} example;

}

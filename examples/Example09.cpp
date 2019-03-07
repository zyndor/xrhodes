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
#include "xr/memory/ScopeGuard.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

// We're adding a Bokeh / Depth of Field effect to the scene from the previous
// example.

// With the increasing number of render passes and different things to render
// in them, we need an abstraction of these items. A Renderable is just a
// a thing that can be rendered, with a material, and at a transform.
class Renderable
{
public:
  Matrix xform;
  Material::Ptr material;

  explicit Renderable(Material::Ptr const& m)
  : material{ m }
  {}

  virtual ~Renderable()
  {}

  void Render() const
  {
    XR_TRANSFORMS_SCOPED_MODEL(xform);
    material->Apply();
    RenderInternal();
  }

  virtual void RenderInternal() const =0;
};

template <typename MeshType, Primitive kPrimitive>
class MeshRenderable: public Renderable
{
public:
  std::shared_ptr<MeshType> mesh;

  MeshRenderable(Material::Ptr const& mt, std::shared_ptr<MeshType> const& ms)
  : Renderable{ mt },
    mesh{ ms }
  {}

  void RenderInternal() const override
  {
    mesh->Render(kPrimitive);
  }
};

template <typename MeshType, Primitive kPrimitive>
class InstancedMeshRenderable: public MeshRenderable<MeshType, kPrimitive>
{
  using BaseType = MeshRenderable<MeshType, kPrimitive>;

public:
  Gfx::InstanceDataBufferHandle hInstances;
  uint16_t numInstances;

  InstancedMeshRenderable(Material::Ptr const& mt,
    std::shared_ptr<MeshType> const& ms,
    Gfx::InstanceDataBufferHandle hInst, uint16_t numInst)
  : BaseType{ mt, ms },
    hInstances{ hInst },
    numInstances{ numInst }
  {}

  void RenderInternal() const override
  {
    Gfx::SetInstanceData(hInstances, 0, numInstances);
    BaseType::RenderInternal();
  }
};

class ScratchRect: public Renderable
{
public:
  Rect mRect;

  ScratchRect(Material::Ptr const& material, Rect rect)
  : Renderable{ material },
    mRect{ rect }
  {}

  virtual void RenderInternal() const override
  {
    auto verts = ScratchBuffer::Start3dUv(4);
    verts->pos = Vector3(mRect.x, mRect.y - mRect.h, 0.f);
    verts->uv0 = Vector2(0.f, 0.f);
    ++verts;
    verts->pos = Vector3(mRect.x + mRect.w, mRect.y - mRect.h, 0.f);
    verts->uv0 = Vector2(1.f, 0.f);
    ++verts;
    verts->pos = Vector3(mRect.x, mRect.y, 0.f);
    verts->uv0 = Vector2(0.f, 1.f);
    ++verts;
    verts->pos = Vector3(mRect.x + mRect.w, mRect.y, 0.f);
    verts->uv0 = Vector2(1.f, 1.f);
    ScratchBuffer::Finish(Primitive::TriStrip);
    ScratchBuffer::Flush();
  }
};

struct Viewer
{
  Matrix worldTransform;
  Matrix4 projection;
};

struct RenderPass
{
  enum
  {
    DEPTH,
    MAIN,
    POST
  };

  Gfx::FrameBufferHandle  hFbo;
  Viewer viewer;
  Rect viewport;
  Gfx::FlagType clearBits;
  float clearDepth;
  Color clearColor;

  // List the things we want rendered in the pass.
  std::vector<std::unique_ptr<Renderable>> renderables;
};

class Example09 : public Example
{
public:
  Example09()
  : Example("09 - Depth of Field")
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

    m_mesh = std::make_shared<Mesh>(Mesh::Create(XR_ARRAY_SIZE(vertices), vertices));

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
      id.pos *= 5.5f * id.scale;

      instanceData.push_back(id);
    }

    m_hInstanceData = Gfx::CreateInstanceDataBuffer({ instanceData.size() * sizeof(InstanceData), reinterpret_cast<uint8_t*>(instanceData.data()) }, sizeof(InstanceData));

    m_uLightColor = Gfx::CreateUniform("uLightColor", Gfx::UniformType::Vec4);

    m_uLightDirection_InvShadowMapSize = Gfx::CreateUniform("uLightDir_InvShadowMapSize", Gfx::UniformType::Vec4);
    m_uLightSpaceTransform =
      Gfx::CreateUniform("uLightSpaceTransform", Gfx::UniformType::Mat4);

    // create uniform for bokeh parameters
    m_uNear_InvDepth = Gfx::CreateUniform("uNear_InvDepth", Gfx::UniformType::Vec4);
    m_uFocalLength_Amount_InvAspectRatio = Gfx::CreateUniform("uFocalLength_Amount_InvAspectRatio", Gfx::UniformType::Vec4);

    Texture::RegisterSamplerUniform("uTexture0", 0);
    Texture::RegisterSamplerUniform("uShadowMap", 1);

    int32_t shadowMapSize = 1024;
    auto hShadowMap = Gfx::CreateTexture(Gfx::TextureFormat::D32,
        shadowMapSize, shadowMapSize, 0, Gfx::F_TEXTURE_FILTER_POINT);

    float orthoSize = 11.5f;
    m_renderPasses[RenderPass::DEPTH] = { // depth pre-pass
      Gfx::CreateFrameBuffer(1, &hShadowMap),
      Viewer{
        Matrix{ Quaternion::FromAxisAngle(Vector3::UnitX(), kPiHalf),
          Vector3(0., orthoSize, -10.) },
      },
      Rect{ 0, 0, shadowMapSize, shadowMapSize },
      Gfx::F_CLEAR_DEPTH,
      1.,
      Color(0.),  // no color target; ignored
    };
    ProjectionHelper::CalculateOrthographic(orthoSize, 0., orthoSize * 2.,
      m_renderPasses[RenderPass::DEPTH].viewer.projection.data);

    auto material = Asset::Manager::Load<Material>("example08-depth.mtl");
    m_renderPasses[RenderPass::DEPTH].renderables.push_back(
      std::make_unique<InstancedMeshRenderable<Mesh, Primitive::TriList>>(material, m_mesh, m_hInstanceData, m_numInstances));
    m_assetQueue.Add(material);

    m_shadowMap = Texture::FromHandle(hShadowMap);

    // This time, our main pass is being rendered into an off-screen buffer.
    // Note the use of an alpha channel - we will be writing normalized depth
    // values into it. The difference between the focal length and this will
    // determine how much each fragment will need to be blurred.
    Gfx::TextureHandle hOffscreen[] = {
      Gfx::CreateTexture(Gfx::TextureFormat::RGBA8, Gfx::GetLogicalWidth(),
        Gfx::GetLogicalHeight(), 0, Gfx::F_TEXTURE_NONE),
      Gfx::CreateTexture(Gfx::TextureFormat::D32, Gfx::GetLogicalWidth(),
        Gfx::GetLogicalHeight(), 0, Gfx::F_TEXTURE_NONE),
    };

    // We only need the depth buffer to allow for depth testing; we
    // don't need to keep a handle to it - clean it up once setup is done.
    auto colorGuard = MakeScopeGuard([h = hOffscreen[1]] {
      Gfx::Release(h);
    });

    m_renderPasses[RenderPass::MAIN] = { // main pass
      Gfx::CreateFrameBuffer(2, hOffscreen),
      Viewer{
        Matrix::Identity(),
      },
      Rect{ 0, 0, Gfx::GetLogicalWidth(), Gfx::GetLogicalHeight() },
      Gfx::F_CLEAR_DEPTH | Gfx::F_CLEAR_COLOR,
      1.,
      // Since we're not rendering a backgroun, clearing our alpha (depth)
      // channel to zero will prevent blur from objects bleeding into it.
      Color(0., 0.125, 0.25, 0.)
    };
    float zNearMain = 0.1f;
    float zFarMain = 25.f;
    ProjectionHelper::CalculatePerspective(kPi * .25f,
      Gfx::GetLogicalAspectRatio(), zNearMain, zFarMain,
      m_renderPasses[RenderPass::MAIN].viewer.projection.data);

    material = Asset::Manager::Load<Material>("example09.mtl");
    m_renderPasses[RenderPass::MAIN].renderables.push_back(
      std::make_unique<InstancedMeshRenderable<Mesh, Primitive::TriList>>(material, m_mesh, m_hInstanceData, m_numInstances));
    m_assetQueue.Add(material);

    // Create pass for post processing.
    m_renderPasses[RenderPass::POST] = { // post processing pass
      Gfx::GetDefaultFrameBuffer(),
      Viewer{
        Matrix::Identity(),
      },
      Rect{ 0, 0, Gfx::GetLogicalWidth(), Gfx::GetLogicalHeight() },
      Gfx::F_CLEAR_DEPTH | Gfx::F_CLEAR_COLOR,
      1.,
      Color(0., 0.125, 0.25),
    };
    ProjectionHelper::CalculateOrthographic(Gfx::GetLogicalWidth(),
      Gfx::GetLogicalHeight(), -10.f, 10.f,
      m_renderPasses[RenderPass::POST].viewer.projection.data);

    // Create post-processing material and transfer ownership of offscreen texture
    // handle to it.
    material = Material::Ptr(Material::Create(0, Asset::UnmanagedFlag)->Cast<Material>());

    auto& postMaterial = material;
    postMaterial->OverrideStateFlags(0, Gfx::F_STATE_CULL_BACK);
    postMaterial->SetTexture(0, Texture::FromHandle(hOffscreen[0]));
    postMaterial->SetShader(Asset::Manager::Load<Shader>("example09-post.shd"));
    m_assetQueue.Add(postMaterial->GetShader());

    // Create whole screen quad to render results of post process pass to.
    auto rect = m_renderPasses[RenderPass::POST].viewport;
    m_renderPasses[RenderPass::POST].renderables.push_back(
      std::make_unique<ScratchRect>(material, rect));

    // Set Bokeh uniform values
    Vector4 nearInvDepth(zNearMain, 1.f / (zFarMain - zNearMain));
    Gfx::SetUniform(m_uNear_InvDepth, nearInvDepth.data);

    Vector4 focalLengthAmountInvAspectRatio(0.333, 1.0, 1.0f / Gfx::GetLogicalAspectRatio());
    Gfx::SetUniform(m_uFocalLength_Amount_InvAspectRatio, focalLengthAmountInvAspectRatio.data);
    Gfx::Flush();

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
        m_renderPasses[RenderPass::MAIN].renderables.back()->material->SetTexture(1, m_shadowMap);
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
      for (auto& rp: m_renderPasses)
      {
        if (&rp != m_renderPasses + RenderPass::POST)
        {
          for (auto& rb: rp.renderables)
          {
            rb->xform = Matrix(m_rotation, Vector3::UnitZ() * -10.f);
          }
        }
      }

      Color lightColor(.75f, .66f + sinf(Timer::GetUST() * .0002f) * .333, .5f);
      Gfx::SetUniform(m_uLightColor, &lightColor);

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

      float lightDirBuffer[4];
      *reinterpret_cast<Vector3*>(lightDirBuffer) = viewer.worldTransform.GetColumn(Vector3::Z);
      lightDirBuffer[3] = 1.f / m_shadowMap->GetWidth();
      Gfx::SetUniform(m_uLightDirection_InvShadowMapSize, lightDirBuffer);
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

      for (auto& renderable: rp.renderables)
      {
        renderable->Render();
      }

      Gfx::Flush();
    }

    Gfx::Present();
  }

  virtual void Shutdown() override
  {
    m_dragControl.DisconnectMouse();

    for (auto& rp: m_renderPasses)
    {
      rp.renderables.clear();
    }

    m_shadowMap.Reset(nullptr);
    m_offscreen.Reset(nullptr);

    m_mesh.reset();

    Gfx::Release(m_hInstanceData);
    m_hInstanceData.Invalidate();

    Gfx::Release(m_renderPasses[RenderPass::DEPTH].hFbo);
    m_renderPasses[RenderPass::DEPTH].hFbo.Invalidate();

    Gfx::Release(m_renderPasses[RenderPass::MAIN].hFbo);
    m_renderPasses[RenderPass::MAIN].hFbo.Invalidate();

    Gfx::Release(m_uLightSpaceTransform);
    m_uLightSpaceTransform.Invalidate();

    Gfx::Release(m_uLightColor);
    m_uLightColor.Invalidate();

    Gfx::Release(m_uLightDirection_InvShadowMapSize);
    m_uLightDirection_InvShadowMapSize.Invalidate();

    Gfx::Release(m_uNear_InvDepth);
    m_uNear_InvDepth.Invalidate();

    Gfx::Release(m_uFocalLength_Amount_InvAspectRatio);
    m_uFocalLength_Amount_InvAspectRatio.Invalidate();

    Asset::Manager::Shutdown();
    Gfx::Shutdown();
  }

private:
  std::shared_ptr<Mesh> m_mesh;
  Gfx::InstanceDataBufferHandle m_hInstanceData;
  uint16_t m_numInstances;

  Gfx::UniformHandle m_uLightSpaceTransform;
  Gfx::UniformHandle m_uLightDirection_InvShadowMapSize;
  Gfx::UniformHandle m_uLightColor;
  Gfx::UniformHandle m_uNear_InvDepth;
  Gfx::UniformHandle m_uFocalLength_Amount_InvAspectRatio;

  AssetQueue m_assetQueue;

  DragController m_dragControl;
  Quaternion m_rotation;

  Texture::Ptr m_shadowMap;
  Texture::Ptr m_offscreen;

  RenderPass  m_renderPasses[3];
} example;

}

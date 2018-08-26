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
#include "xr/Timer.hpp"
#include "xr/ScratchBuffer.hpp"
#include "xr/Material.hpp"
#include "xr/Asset.hpp"
#include "xr/math/Quaternion.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

// Introducing the asset system: we are loading a material and its dependencies:
// a texture, and a shader (comprised of its vertex and fragment shader components),
// then use and re-use the ScratchBuffer to draw multiple textured quads, using
// alpha blending (which is defined by the material).
class Example03 : public Example
{
public:
  Example03()
  : Example("03 - Asset Loading")
  {}

  virtual void Init() override
  {
    Gfx::Init(Device::GetGfxContext());
    ScratchBuffer::Init(64);

    // Initialize asset manager.
    Asset::Manager::Init(".assets");

    // Use Texture's helper to register a sampler uniform which will be automatically
    // released when Gfx is shut down.
    Texture::RegisterSamplerUniform("uTexture0", 0);

    // Load material with alpha blended XRhodes logo,asynchronously.
    m_loading = true;
    m_material = Asset::Manager::Load<Material>("example03.mtl");

    XR_TRACE(Example03, ("Loading..."));

    m_dragControl.Reset();
    m_dragControl.ConnectMouse();
  }

  virtual void Update() override
  {
    if (m_loading)
    {
      // Poll the asset manager.
      Asset::Manager::Update();

      // Check if we've finished loading.
      if (CheckAllMaskBits(m_material->GetFlags(), Asset::ReadyFlag))
      {
        m_loading = false;
        XR_TRACE(Example03, ("Loading complete!"));
      }
    }
    else
    {
      m_dragControl.Update();
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

    // Draw 4 textured quads reusing the ScratchBuffer.
    float aspectRatio = Gfx::GetLogicalHeight() * (1.f / Gfx::GetLogicalWidth());
    float halfHeight = .5f;
    float halfWidth = halfHeight * aspectRatio;

    const Vector2 offsets[] = {
      Vector2(-.125f, -.25f),
      Vector2(.25f, -.125f),
      Vector2(.125f, .25f),
      Vector2(-.25f, .125f),
    };

    auto motion = m_dragControl.GetTotalMotion() * 0.01f;
    for (int i = 0; i < XR_ARRAY_SIZE(offsets); ++i)
    {
      float c = std::cos(motion.x);
      float s = std::sin(motion.x);
      Vector2 offset(c * offsets[i].x - s * offsets[i].y,
        s * offsets[i].x + c * offsets[i].y);

      auto vertices = ScratchBuffer::Start2dUv(4);
      vertices[0].pos = Vector2(-halfWidth, halfHeight) + offset;
      vertices[0].uv0 = Vector2(0.f, 1.f);
      vertices[1].pos = Vector2(halfWidth, halfHeight) + offset;
      vertices[1].uv0 = Vector2(1.f, 1.f);
      vertices[2].pos = Vector2(-halfWidth, -halfHeight) + offset;
      vertices[2].uv0 = Vector2(0.f, 0.f);
      vertices[3].pos = Vector2(halfWidth, -halfHeight) + offset;
      vertices[3].uv0 = Vector2(1.f, 0.f);
      ScratchBuffer::Finish(Primitive::TriStrip);

      // Reuse scratch buffer. NOTE: Flush is automatically called when Gfx is
      // Flush()ed or Presented().
      ScratchBuffer::Flush();
    }

    Gfx::Present();
  }

  virtual void Shutdown() override
  {
    m_dragControl.DisconnectMouse();

    // NOTE: having the material on the same scope as the Asset::Manager
    // means that we need to release it manually.
    m_material.Reset(nullptr);

    // Kill Asset Manager before Gfx, since Gfx will report leaks for the
    // outstanding textures and shaders, and trying to destroy them after
    // Gfx has shut down will lead to a crash.
    Asset::Manager::Shutdown();

    Gfx::Shutdown();
  }

private:

  bool m_loading;
  Material::Ptr  m_material;

  DragController m_dragControl;
} example;

}

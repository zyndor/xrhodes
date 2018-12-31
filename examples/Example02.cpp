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
#include "xr/Image.hpp"
#include "xr/Input.hpp"
#include "xr/Device.hpp"
#include "xr/Timer.hpp"
#include "xr/ScratchBuffer.hpp"
#include "xr/math/Quaternion.hpp"
#include "xr/events/Callback.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

auto kVertShader = "#version 330\n"
XR_STRINGIFY(
  precision mediump float;
  uniform vec4 uRotation;
  in vec2 aPosition;
  in vec2 aUV0;
  out vec2 vUv;

  vec3 QuaternionRotate(vec3 p, vec4 q)
  {
    return p + 2.f * cross(q.xyz, cross(q.xyz, p) + q.w * p);
  }

  void main()
  {
    vUv = aUV0;
    vec2 pos = QuaternionRotate(vec3(aPosition, 0.f), uRotation).xy;
    gl_Position = vec4(pos, 0.f, 1.f);
  });

auto kFragShader = "#version 330\n"
XR_STRINGIFY(
  precision mediump float;
  uniform sampler2D uTexture0;
  in vec2 vUv;
  out vec4 FragColor;
  void main()
  {
    FragColor = texture(uTexture0, vUv);
  });

// We are loading an image file, creating a texture from its pixel data, and
// drawing a textured quad.
class Example02 : public Example
{
public:
  Example02()
  : Example("02 - Textures")
  {}

  virtual void Init() override
  {
    Gfx::Init(Device::GetGfxContext());
    ScratchBuffer::Init(64);

    // Create a uniform for the rotation of the image.
    m_uRotation = Gfx::CreateUniform("uRotation", Gfx::UniformType::Vec4);

    // Create a uniform for our texture sampler.
    m_uTexture0 = Gfx::CreateUniform("uTexture0", Gfx::UniformType::Int1);

    // We'll set it once, since it will not change in this example.
    int textureStage = 0;
    Gfx::SetUniform(m_uTexture0, &textureStage);

    auto hVert = Gfx::CreateShader(Gfx::ShaderType::Vertex,
      { strlen(kVertShader), reinterpret_cast<const uint8_t*>(kVertShader) });
    auto hFrag = Gfx::CreateShader(Gfx::ShaderType::Fragment,
      { strlen(kFragShader), reinterpret_cast<const uint8_t*>(kFragShader) });
    m_hShader = Gfx::CreateProgram(hVert, hFrag);

    Gfx::Release(hVert);
    Gfx::Release(hFrag);
    Gfx::Flush();

    // Load an image and create a texture from its pixel data.
    Image img;
    if (img.Load("xrhodes.png"))
    {
      Buffer buffer = { img.GetPixelDataSize(), img.GetPixelData() };
      m_hTexture = Gfx::CreateTexture(Gfx::TextureFormat::RGBA8, img.GetWidth(),
        img.GetHeight(), 0, Gfx::F_TEXTURE_FILTER_POINT, &buffer);
    }

    // Reset motion
    m_motion = Vector2::Zero();

    // Register for mouse events to the DragController
    m_dragControl.ConnectMouse();
  }

  virtual void Update() override
  {
    // update rotor, obtain a quaternion representing the rotation around the
    // x and y axes, then update the uniform with it.
    m_dragControl.Update();

    auto motion = m_dragControl.GetFrameMotion() * .01f;
    m_motion += motion;
    auto rotation =
      Quaternion::FromAxisAngle(Vector3::UnitX(), -m_motion.y) *
      Quaternion::FromAxisAngle(Vector3::UnitY(), -m_motion.x);
    Gfx::SetUniform(m_uRotation, rotation.data);
  }

  virtual void Render() override
  {
    Gfx::Clear(Gfx::F_CLEAR_COLOR | Gfx::F_CLEAR_DEPTH, Color(0.f, .25f, .5f));

    // Request alpha blending
    Gfx::SetState(Gfx::F_STATE_ALPHA_BLEND);

    // Bind the texture
    Gfx::SetTexture(m_hTexture);

    Gfx::SetProgram(m_hShader);

    // Draw a textured quad using the ScratchBuffer.
    float aspectRatio = Gfx::GetLogicalHeight() * (1.f / Gfx::GetLogicalWidth());
    float halfHeight = .25f;
    float halfWidth = halfHeight * aspectRatio;

    auto vertices = ScratchBuffer::Start2dUv(4);
    vertices[0].pos = Vector2(-halfWidth, halfHeight);
    vertices[0].uv0 = Vector2(0.f, 1.f);
    vertices[1].pos = Vector2(halfWidth, halfHeight);
    vertices[1].uv0 = Vector2(1.f, 1.f);
    vertices[2].pos = Vector2(-halfWidth, -halfHeight);
    vertices[2].uv0 = Vector2(0.f, 0.f);
    vertices[3].pos = Vector2(halfWidth, -halfHeight);
    vertices[3].uv0 = Vector2(1.f, 0.f);
    ScratchBuffer::Finish(Primitive::TriStrip);

    // Present the back buffer.
    Gfx::Present();
  }

  virtual void Shutdown() override
  {
    // Disconmnect from mouse signals; no more input, we're shutting down.
    m_dragControl.DisconnectMouse();

    // Release and invalidate the texture handle.
    Gfx::Release(m_hTexture);
    m_hTexture.Invalidate();

    // Release and invalidate the sampler handle.
    Gfx::Release(m_uTexture0);
    m_uTexture0.Invalidate();

    Gfx::Release(m_hShader);
    m_hShader.Invalidate();

    Gfx::Release(m_uRotation);
    m_uRotation.Invalidate();

    Gfx::Shutdown();
  }

private:
  Gfx::UniformHandle m_uRotation;
  Gfx::ProgramHandle m_hShader;
  Gfx::UniformHandle m_uTexture0;
  Gfx::TextureHandle m_hTexture;

  DragController m_dragControl;
  Vector2 m_motion;
} example;

}

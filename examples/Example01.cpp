//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Example.hpp"
#include "xr/Device.hpp"
#include "xr/Timer.hpp"
#include "xr/ScratchBuffer.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

// Vertex shader source. Renders vertices comprising of position and color,
// applying a sinusoidal based on position.
auto kVertShader = "#version 330\n"
XR_STRINGIFY(
  precision mediump float;
  uniform vec4 uTime;
  in vec2 aPosition;  // attributes from the Gfx vertex format
  in vec4 aColor0;
  out vec4 vColor;

  void main()
  {
    vColor = aColor0 + sin(uTime.x + aPosition.y) * .25f;
    gl_Position = vec4(aPosition, 0.f, 1.f);
  });

auto kFragShader = "#version 330\n"
XR_STRINGIFY(
  precision mediump float;
  in vec4 vColor;

  void main()
  {
    gl_FragColor = vColor;
  });

// We'll initialise the graphics and scratch buffer, then create a uniform
// and a shader to draw a pulsating X with.
class Example01 : public Example
{
public:
  Example01()
  : Example("01 - Hello XRhodes")
  {}

  virtual void Init() override
  {
    // Initialise the graphics subsystem.
    Gfx::Init(Device::GetGfxContext());

    // Initialise ScratchBuffer for drawing. The memory will be sufficient for
    // a single 2d textured quad.
    // This system will shut down when Gfx does.
    ScratchBuffer::Init(96);

    // Register a uniform for our shader. The creation of uniforms must
    // precede the creation of the shaders that use them. All uniforms are
    // available for all shaders, therefore their type must match that
    // of the registered uniform.
    m_uTime = Gfx::CreateUniform("uTime", Gfx::UniformType::Vec4);

    // Create shader pipeline using vertex and fragment shader sources.
    auto hVert = Gfx::CreateShader(Gfx::ShaderType::Vertex,
      { strlen(kVertShader), reinterpret_cast<const uint8_t*>(kVertShader) });
    auto hFrag = Gfx::CreateShader(Gfx::ShaderType::Fragment,
      { strlen(kFragShader), reinterpret_cast<const uint8_t*>(kFragShader) });
    m_hShader = Gfx::CreateProgram(hVert, hFrag);

    // After the program has been created, it shares ownership of the
    // shader handles; we need to release our local copies.
    Gfx::Release(hVert);
    Gfx::Release(hFrag);
  }

  virtual void Update() override
  {
  }

  virtual void Render() override
  {
    // Clear the buffers.
    Gfx::Clear(Gfx::F_CLEAR_COLOR | Gfx::F_CLEAR_DEPTH, Color(0.f, .25f, .5f));

    // Update the value of the uniform.
    float time[4] = { Timer::GetUST() * .005f };
    Gfx::SetUniform(m_uTime, time);

    // Set our program.
    Gfx::SetProgram(m_hShader);

    // Draw an X using the ScratchBuffer.
    float aspectRatio = Gfx::GetLogicalHeight() * (1.f / Gfx::GetLogicalWidth());
    float halfHeight = .25f;
    float halfWidth = halfHeight * aspectRatio;

    auto vertices = ScratchBuffer::Start2dColor(4);
    vertices[0].pos = Vector2(-halfWidth, halfHeight);
    vertices[0].color0 = Color(1.f, .5f, .5f);
    vertices[1].pos = Vector2(halfWidth, -halfHeight);
    vertices[1].color0 = Color(1.f, 0.f, .5f);
    vertices[2].pos = Vector2(-halfWidth, -halfHeight);
    vertices[2].color0 = Color(1.f, 1.f, 0.f);
    vertices[3].pos = Vector2(halfWidth, halfHeight);
    vertices[3].color0 = Color(1.f, 1.f, .5f);
    ScratchBuffer::Finish(Primitive::LineList);

    // Present the back buffer.
    Gfx::Present();
  }

  virtual void Shutdown() override
  {
    // Release and invalidate the program handle.
    Gfx::Release(m_hShader);
    m_hShader.Invalidate();

    // Release and invalidate the uniform handle.
    Gfx::Release(m_uTime);
    m_uTime.Invalidate();

    Gfx::Shutdown();
  }

private:
  Gfx::UniformHandle m_uTime;
  Gfx::ProgramHandle m_hShader;
} example;

}

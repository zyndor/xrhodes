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
#include "xr/debugdraw.hpp"
#include "xr/ScratchBuffer.hpp"
#include "xr/BoxText.hpp"
#include "xr/IndexMesh.hpp"
#include "xr/Timer.hpp"
#include "xr/Material.hpp"
#include "xr/Font.hpp"
#include "xr/Vertex.hpp"
#include "xr/Transforms.hpp"
#include "xr/FileBuffer.hpp"
#include "xr/Asset.hpp"
#include "xr/Device.hpp"
#include "xr/Input.hpp"
#include "xr/math/Vector4.hpp"
#include "xr/math/Quaternion.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

// We're loading a font and creating text blocks with it.
class Example05 : public Example
{
public:
  Example05()
  : Example("05 - Fonts and Text")
  {}

  virtual void Init() override
  {
    Gfx::Init(Device::GetGfxContext());
    Asset::Manager::Init(".assets");

    Transforms::Init();

    // DebugDraw requires the ScratchBuffer, so initialise it.
    ScratchBuffer::Init(128);

    m_rotation = Quaternion::Identity();

    m_uTime = Gfx::CreateUniform("uTime", Gfx::UniformType::Vec4);

    Texture::RegisterSamplerUniform("uTexture0", 0);
    Gfx::Flush();

    m_text3dSize = Vector2(350.f, 300.f);
    m_text3dVerticalAlignment = BoxText::Alignment::Near;
    m_text3dHorizontalAlignment = BoxText::Alignment::Near;

    // Load a text file whose contents we'll display.
    m_buffer.Open("example05.txt");
    m_buffer.Close();

    m_loading = true;

    // Load a font and the shader to render it with.
    m_font = Asset::Manager::Load<Font>("lucon.fnt");
    m_text2dShader = Asset::Manager::Load<Shader>("example05.shd");
    m_text3dShader = Asset::Manager::Load<Shader>("example05-wave.shd");

    m_rotation = Quaternion::Identity();
    m_dragControl.Reset();
    m_dragControl.ConnectMouse();
  }

  virtual void Update() override
  {
    if (m_loading)
    {
      Asset::Manager::Update();
      bool loading = !(CheckAllMaskBits(m_font->GetFlags() &
        m_text2dShader->GetFlags() & m_text3dShader->GetFlags(), Asset::ReadyFlag));
      if (!loading)
      {
        // Once successfully loaded, create material from font, using shader.
        auto state = Gfx::F_STATE_ALPHA_BLEND;
        m_text3dMaterial = m_font->Materialise(state, m_text3dShader);
        m_text2dMaterial = m_font->Materialise(state, m_text2dShader);


        // Create 2D text block
        BoxText text;
        text.SetScale(.5f);
        text.SetFont(m_font);
        text.EnsureVerticalFit();
        text.SetHorizontalAlignment(BoxText::Alignment::Near);
        text.SetVerticalAlignment(BoxText::Alignment::Near);
        text.SetOrigin(Vector2(.5f, -.5f));

        m_text2dMesh = text.CreateMesh("Press TAB to cycle through alignment options. Cursor keys adjust box size.", true, nullptr);

        // Create 3D text block
        CreateText3d();

        m_loading = loading;
      }
    }
    else
    {
      Vector4 time(Timer::GetUST());
      Gfx::SetUniform(m_uTime, time.data);

      m_dragControl.Update();

      float weight = 1.f - m_dragControl.GetPressTimerPercent();

      const float controlPoints[] = { 0.f, .12f, .88f, 1.f };
      weight = Bezier(controlPoints, weight);

      auto motion = m_dragControl.GetFrameMotion() * 0.02f;
      m_rotation *= Quaternion::FromAxisAngle(Vector3(.2, .5, 0.f), weight * .005f) *
        Quaternion::FromPitchYawRoll(motion.x, motion.y, .0);

      if (Input::GetKeyState(K_TAB) == ButtonState::WasPressed)
      {
        int align = (static_cast<int>(m_text3dHorizontalAlignment) + 3 * static_cast<int>(m_text3dVerticalAlignment) + 1) % 9;
        m_text3dVerticalAlignment = static_cast<BoxText::Alignment>(align / 3);
        m_text3dHorizontalAlignment = static_cast<BoxText::Alignment>(align % 3);

        CreateText3d();
      }

      const float dSize = 6.f;
      bool rebuildText = false;
      if (Input::GetKeyState(K_LEFT) == ButtonState::Down && m_text3dSize.x - dSize > 0.f)
      {
        m_text3dSize.x -= dSize;
        rebuildText = true;
      }

      if (Input::GetKeyState(K_RIGHT) == ButtonState::Down)
      {
        m_text3dSize.x += dSize;
        rebuildText = true;
      }

      if (Input::GetKeyState(K_UP) == ButtonState::Down && m_text3dSize.y - dSize > 0.f)
      {
        m_text3dSize.y -= dSize;
        rebuildText = true;
      }

      if (Input::GetKeyState(K_DOWN) == ButtonState::Down)
      {
        m_text3dSize.y += dSize;
        rebuildText = true;
      }

      if (rebuildText)
      {
        CreateText3d();
      }
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

    // Render 3D text.
    Transforms::Updater().SetPerspectiveProjection(kPi * .25f, .1f, 100.f);

    {
      Matrix m(m_rotation, Vector3::UnitZ() * -50.f);
      m.ScaleLinear((1.5 + std::sin(Timer::GetUST() * .0001f) * .5) / 15.f);
      XR_TRANSFORMS_SCOPED_MODEL(m);

      m_text3dMaterial->Apply();
      m_text3dMesh.Render(Primitive::TriangleList);

      // Use DebugDraw to visualize the border of the 3D text area.
      DebugDraw::SetColor(Color(0xff44ffbb));
      DebugDraw::Rect(m_text3dSize.x * .5f, m_text3dSize.y * .5f);
    }

    // Switch to orthographics projection, render 2D text.
    Transforms::Updater().SetOrthographicProjection(0.f, Gfx::GetLogicalWidth(),
      -Gfx::GetLogicalHeight(), 0.f, -100.f, 100.f);

    {
      XR_TRANSFORMS_SCOPED_MODEL(Matrix::Identity());

      m_text2dMaterial->Apply();
      m_text2dMesh.Render(Primitive::TriangleList);
    }

    Gfx::Present();
  }

  virtual void Shutdown() override
  {
    m_text2dMesh = IndexMesh();
    m_text3dMesh = IndexMesh();

    m_text2dMaterial.Reset(nullptr);
    m_text2dShader.Reset(nullptr);

    m_text3dMaterial.Reset(nullptr);
    m_text3dShader.Reset(nullptr);

    m_font.Reset(nullptr);

    Gfx::Release(m_uTime);
    m_uTime.Invalidate();

    Asset::Manager::Shutdown();
    Gfx::Shutdown();
  }

private:

  Gfx::UniformHandle m_uTime;

  Quaternion m_rotation;

  FileBuffer m_buffer;

  bool m_loading;
  Font::Ptr m_font;
  Shader::Ptr m_text3dShader;
  Shader::Ptr m_text2dShader;

  Vector2 m_text3dSize;
  BoxText::Alignment m_text3dVerticalAlignment;
  BoxText::Alignment m_text3dHorizontalAlignment;

  Material::Ptr m_text3dMaterial;
  IndexMesh m_text3dMesh;

  Material::Ptr m_text2dMaterial;
  IndexMesh m_text2dMesh;

  DragController m_dragControl;
  Vector2 m_motion;

  void CreateText3d()
  {
    BoxText text;
    text.SetFont(m_font);
    text.SetBoxSize(m_text3dSize.x, m_text3dSize.y);
    text.SetHorizontalAlignment(m_text3dHorizontalAlignment);
    text.SetVerticalAlignment(m_text3dVerticalAlignment);
    text.SetHorizontalSpacing(3.f);
    text.SetVerticalSpacing(-2.f);

    m_text3dMesh = text.CreateMesh(m_buffer.CastData<char const>(),
      m_buffer.GetSize(), true, nullptr);
  }
} example;

}

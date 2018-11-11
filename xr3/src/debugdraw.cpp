//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/debugdraw.hpp"
#include "xr/ScratchBuffer.hpp"

namespace xr
{
namespace DebugDraw
{

//==============================================================================
namespace
{

Gfx::UniformHandle s_uColor;
Material::Ptr  s_material;

char const* const kVertexShader = "#version 300 es\n"
XR_STRINGIFY(

precision mediump float;

in vec3 aPosition;

uniform mat4 xruModelViewProjection;

void main()
{
  gl_Position = xruModelViewProjection * vec4(aPosition, 1.0);
}
);

char const* const kFragmentShader = "#version 300 es\n"
XR_STRINGIFY(
precision mediump float;

uniform vec4 xruColor;

out vec4 fragColor;

void main()
{
  fragColor = xruColor;
}
);

//==============================================================================
void EnsureUniformExists()
{
  if(!s_uColor.IsValid())
  {
    s_uColor = Gfx::CreateUniform("xruColor", Gfx::UniformType::Vec4);
    Gfx::ShutdownSignal().Connect(FunctionPtrCallback<void>([](void*) {
      Gfx::Release(s_uColor);
      s_uColor.Invalidate();
    }, nullptr));
  }
}

void SetMaterial(Material::Ptr material)
{
  if (!material)
  {
    if (!s_material)
    {
      EnsureUniformExists();

      Asset::FlagType flags = Asset::UnmanagedFlag;
      s_material.Reset(Material::Create(0, flags)->Cast<Material>());

      ShaderComponent::Ptr vertexShader(ShaderComponent::Create(0, flags)->Cast<ShaderComponent>());
      vertexShader->SetSource(Gfx::ShaderType::Vertex, kVertexShader);

      ShaderComponent::Ptr fragmentShader(ShaderComponent::Create(0, flags)->Cast<ShaderComponent>());
      fragmentShader->SetSource(Gfx::ShaderType::Fragment, kFragmentShader);

      Shader::Ptr shader(Shader::Create(0, flags)->Cast<Shader>());
      shader->SetComponents(vertexShader, fragmentShader);

      s_material->SetShader(shader);

      Gfx::ShutdownSignal().Connect(FunctionPtrCallback<void>([](void*) {
        s_material.Reset(nullptr);
      }, nullptr));
    }

    material = s_material;
  }
  material->Apply();
}

void  DrawLineInternal(const Vector3* positions, int numVerts, Material::Ptr const& material,
  Primitive primitive)
{
  SetMaterial(material);
  auto verts = ScratchBuffer::Start3d(numVerts);
  auto end = verts + numVerts;
  while (verts != end)
  {
    verts->pos = *positions;
    ++positions;
    ++verts;
  }

  ScratchBuffer::Finish(primitive);
}

uint32_t GetCircleNumVerts(float radius)
{
  XR_ASSERT(GetCircleNumVerts, radius >= .0f);
  return static_cast<uint32_t>(std::ceil(kPi * radius * .333f));
}

} // namespace

//==============================================================================
void  SetColor(Color const& color)
{
  EnsureUniformExists();
  Gfx::SetUniform(s_uColor, &color);
}

//==============================================================================
void  Line(const Vector3& v, Material::Ptr const& material)
{
  SetMaterial(material);

  auto verts = ScratchBuffer::Start3d(2);
  verts[0].pos = Vector3::Zero();
  verts[1].pos = v;

  ScratchBuffer::Finish(Primitive::LineStrip);
}

//==============================================================================
void  LineStrip(const Vector3* positions, int numVerts, Material::Ptr const& material)
{
  DrawLineInternal(positions, numVerts, material, Primitive::LineStrip);
}

//==============================================================================
void  LineList(const Vector3* positions, int numVerts, Material::Ptr const& material)
{
  DrawLineInternal(positions, numVerts, material, Primitive::LineList);
}

//==============================================================================
void  Rect(float hw, float hh, Material::Ptr const& material)
{
  SetMaterial(material);
  auto verts = ScratchBuffer::Start3d(5);
  verts[0].pos = Vector3(-hw, -hh, .0f);
  verts[1].pos = Vector3(-hw, hh, .0f);
  verts[2].pos = Vector3(hw, hh, .0f);
  verts[3].pos = Vector3(hw, -hh, .0f);
  verts[4] = verts[0];

  ScratchBuffer::Finish(Primitive::LineStrip);
}

//==============================================================================
void  FillRect(float hw, float hh, Material::Ptr const& material)
{
  SetMaterial(material);
  auto verts = ScratchBuffer::Start3d(4);
  verts[0].pos = Vector3(-hw, -hh, .0f);
  verts[1].pos = Vector3(-hw, hh, .0f);
  verts[2].pos = Vector3(hw, -hh, .0f);
  verts[3].pos = Vector3(hw, hh, .0f);

  ScratchBuffer::Finish(Primitive::TriStrip);
}

//==============================================================================
void  Circle(float radius, Material::Ptr const& material)
{
  uint32_t numVerts = GetCircleNumVerts(radius);
  float theta = float(M_PI / numVerts);
  float c = std::cos(theta);
  float s = std::sin(theta);
  Vector3 v(radius, .0f, .0f);

  numVerts *= 2;
  ++numVerts;

  SetMaterial(material);
  auto verts = ScratchBuffer::Start3d(numVerts);
  auto endVerts = verts + numVerts;
  while (verts != endVerts)
  {
    v = Vector3(v.x * c + v.y * s, v.y * c - v.x * s, .0f);
    verts->pos = v;
    ++verts;
  }

  ScratchBuffer::Finish(Primitive::LineStrip);
}

//==============================================================================
void  FillCircle(float radius, Material::Ptr const& material)
{
  SetMaterial(material);

  uint32_t numVerts = GetCircleNumVerts(radius);
  float theta = float(M_PI / numVerts);
  float c = cosf(theta);
  float s = sinf(theta);
  Vector3 v(radius, .0f, .0f);

  numVerts *= 2;
  auto verts = ScratchBuffer::Start3d(numVerts);
  auto endVerts = verts + numVerts - 1;
  verts->pos = v;
  ++verts;
  while (verts != endVerts)
  {
    v = Vector3(v.x * c + v.y * s, v.y * c - v.x * s, .0f);
    verts->pos = Vector3(v.x, -v.y, .0f);
    ++verts;
    verts->pos = v;
    ++verts;
  }

  verts->pos = Vector3(-radius, .0f, .0f);

  ScratchBuffer::Finish(Primitive::TriStrip);
}

} // DebugDraw

} // xr

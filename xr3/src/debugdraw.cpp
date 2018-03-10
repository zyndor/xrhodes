//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/debugdraw.hpp"
#include "XR/ScratchBuffer.hpp"

namespace XR
{

namespace DebugDraw
{

//==============================================================================
namespace
{

Gfx::UniformHandle s_xruColor;
Material::Ptr  s_material;

#define XR_STRINGIFY(x) #x

char const* const kVertexShader = "#version 300 es"
XR_STRINGIFY(

in vec3 aPosition;

uniform mat4 xruModelViewProjection;

void main()
{
  gl_Position = xruModelViewProjection * aPosition;
}
);

char const* const kFragmentShader = "#version 300 es"
XR_STRINGIFY(

uniform vec4 s_xruColor;

out vec4 fragColor;

void main()
[
  fragColor = xruColor;
}
);

void SetMaterial(Material::Ptr material)
{
  if (!material)
  {
    if (!s_material)
    {
      s_xruColor = Gfx::CreateUniform("s_xruColor", Gfx::UniformType::Vec4);

      Asset::FlagType flags = Asset::UnmanagedFlag;
      s_material.Reset(Material::Create(0, flags)->Cast<Material>());

      ShaderComponent::Ptr vertexShader(ShaderComponent::Create(0, flags)->Cast<ShaderComponent>());
      vertexShader->SetSource(Gfx::ShaderType::Vertex, kVertexShader);

      ShaderComponent::Ptr fragmentShader(ShaderComponent::Create(0, flags)->Cast<ShaderComponent>());
      fragmentShader->SetSource(Gfx::ShaderType::Fragment, kFragmentShader);

      Shader::Ptr shader(Shader::Create(0, flags)->Cast<Shader>());
      shader->SetComponents(vertexShader, fragmentShader);

      s_material->SetShader(shader);
    }

    material = s_material;
  }
  material->Apply();
}

void  DrawLineInternal(const Vector3* positions, int numVerts, Material::Ptr const& material,
  PrimType primitive)
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
void  Line(const Vector3& v, Material::Ptr const& material)
{
  SetMaterial(material);

  auto verts = ScratchBuffer::Start3d(2);
  verts[0].pos = Vector3::Zero();
  verts[1].pos = v;

  ScratchBuffer::Finish(PrimType::LINE_STRIP);
}

//==============================================================================
void  LineStrip(const Vector3* positions, int numVerts, Material::Ptr const& material)
{
  DrawLineInternal(positions, numVerts, material, PrimType::LINE_STRIP);
}

//==============================================================================
void  LineList(const Vector3* positions, int numVerts, Material::Ptr const& material)
{
  DrawLineInternal(positions, numVerts, material, PrimType::LINE_LIST);
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

  ScratchBuffer::Finish(PrimType::LINE_STRIP);
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

  ScratchBuffer::Finish(PrimType::TRI_STRIP);
}

//==============================================================================
void  Circle(float radius, Material::Ptr const& material)
{
  uint32_t numVerts = GetCircleNumVerts(radius);
  float theta = float(M_PI / numVerts);
  float c = std::cosf(theta);
  float s = std::sinf(theta);
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

  ScratchBuffer::Finish(PrimType::LINE_STRIP);
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

  ScratchBuffer::Finish(PrimType::TRI_STRIP);
}

} // DebugDraw

} // XR

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <algorithm>
#include "xr/utils.hpp"
#include "xr/Sprite.hpp"

namespace xr
{

//==============================================================================
const AABB Sprite::kWholeTexture = { .0f, .0f, 1.0f, 1.0f };
const AABB Sprite::kNullTexture =  { .0f, .0f, .0f, .0f };

//==============================================================================
void Sprite::Slice(AABB base, uint32_t textureWidth, uint32_t textureHeight,
  uint32_t across, uint32_t down, uint32_t maxSlices, Sprite* sprites)
{
  XR_ASSERT(Sprite, textureWidth > 0);
  XR_ASSERT(Sprite, textureHeight > 0);
  if (maxSlices <= 0)
  {
    maxSlices = down * across;
  }

  const float leftStart = base.left;
  const float width = base.right - base.left;
  const float height = base.bottom - base.top;
  decltype(maxSlices) slices = 0;
  for (decltype(down) i = 0; i < down && slices < maxSlices; ++i)
  {
    for (decltype(across) j = 0; j < across && slices < maxSlices; ++j)
    {
      sprites->SetUVsProportional(base, textureWidth, textureHeight);

      base.left = base.right;
      base.right += width;
      ++slices;
      ++sprites;
    }
    base.left = leftStart;
    base.right = leftStart + width;
    base.top = base.bottom;
    base.bottom += height;
  }
}

//==============================================================================
void Sprite::CalculateUVs(AABB const& aabb, bool rotate, Vertex verts[Quad::Vertex::kCount])
{
  if(rotate)
  {
    verts[Quad::Vertex::NW].uv0 = Vector2(aabb.right, aabb.top);
    verts[Quad::Vertex::SW].uv0 = Vector2(aabb.left, aabb.top);
    verts[Quad::Vertex::SE].uv0 = Vector2(aabb.left, aabb.bottom);
    verts[Quad::Vertex::NE].uv0 = Vector2(aabb.right, aabb.bottom);
  }
  else
  {
    verts[Quad::Vertex::NW].uv0 = Vector2(aabb.left, aabb.top);
    verts[Quad::Vertex::SW].uv0 = Vector2(aabb.left, aabb.bottom);
    verts[Quad::Vertex::SE].uv0 = Vector2(aabb.right, aabb.bottom);
    verts[Quad::Vertex::NE].uv0 = Vector2(aabb.right, aabb.top);
  }
}

//==============================================================================
bool Sprite::IsUVRotated(Vertex const verts[Quad::Vertex::kCount])
{
  return verts[Quad::Vertex::NW].uv0.y != verts[Quad::Vertex::NE].uv0.y &&
    verts[Quad::Vertex::NW].uv0.x != verts[Quad::Vertex::SW].uv0.x;
}

//==============================================================================
void Sprite::CopyIndicesTo(uint16_t indices[Quad::kIndexCount])
{
  XR_ASSERT(Sprite, indices != 0);
  memcpy(indices, Quad::kIndices, sizeof(Quad::kIndices));
}

//==============================================================================
Sprite::Sprite()
: m_halfWidth(.0f),
  m_halfHeight(.0f),
  m_isUVRotated(false),
  m_offset(Vector2::Zero())
{}

//==============================================================================
Sprite::~Sprite()
{}

//==============================================================================
void Sprite::SetHalfSize(float hw, float hh, bool calculateVertices)
{
  XR_ASSERT(Sprite, hw >= 0);
  XR_ASSERT(Sprite, hh >= 0);

  m_halfWidth = hw;
  m_halfHeight = hh;

  if (calculateVertices)
  {
    m_vertices[Quad::Vertex::NW].pos = Vector3(-hw, -hh, .0f);
    m_vertices[Quad::Vertex::SW].pos = Vector3(-hw, hh, .0f);
    m_vertices[Quad::Vertex::SE].pos = Vector3(hw, hh, .0f);
    m_vertices[Quad::Vertex::NE].pos = Vector3(hw, -hh, .0f);
  }
}

//==============================================================================
void Sprite::AddOffset(float x, float y, bool updateVertices)
{
  if (updateVertices)
  {
    for (auto& v: m_vertices)
    {
      v.pos.x += x;
      v.pos.y += y;
    }
  }

  m_offset.x += x;
  m_offset.y += y;
}

//==============================================================================
void Sprite::Import(Vertex const verts[Quad::Vertex::kCount])
{
  std::copy(verts, verts + Quad::Vertex::kCount, m_vertices);

  // determine uv rotation
  const bool isUVRotated = IsUVRotated(verts);
  m_isUVRotated = isUVRotated;

  // determine halfsize
  if (isUVRotated)
  {
    m_halfWidth = (verts[Quad::Vertex::SE].pos - verts[Quad::Vertex::NE].pos).Magnitude() * .5f;
    m_halfHeight = (verts[Quad::Vertex::SW].pos - verts[Quad::Vertex::SE].pos).Magnitude() * .5f;
  }
  else
  {
    m_halfWidth = (verts[Quad::Vertex::SE].pos - verts[Quad::Vertex::SW].pos).Magnitude() * .5f;
    m_halfHeight = (verts[Quad::Vertex::NW].pos - verts[Quad::Vertex::SW].pos).Magnitude() * .5f;
  }

  // determine offset
  auto vertsEnd = verts + Quad::Vertex::kCount;
  Vector2 pos(.0f, .0f);
  while (verts != vertsEnd)
  {
    pos += verts->pos.XY();
    ++verts;
  }
  pos *= 1.0f / Quad::Vertex::kCount;
  m_offset = pos;
}

//==============================================================================
void Sprite::SetUVs(const AABB& tc)
{
  m_isUVRotated = false;
  CalculateUVs(tc, false, m_vertices);
}

//==============================================================================
void Sprite::SetUVsProportional(const AABB& uvs, uint32_t textureWidth, uint32_t textureHeight)
{
  XR_ASSERT(Sprite, textureWidth > 0);
  XR_ASSERT(Sprite, textureHeight > 0);
  SetUVs(uvs);

  float hw = (uvs.right - uvs.left) * .5f * textureWidth;
  float hh = (uvs.bottom - uvs.top) * .5f * textureHeight;
  SetHalfSize(hw, hh, true);
}

//==============================================================================
void Sprite::SetUVsRotated(const AABB& tc)
{
  m_isUVRotated = true;
  CalculateUVs(tc, true, m_vertices);
}

//==============================================================================
void Sprite::SetUVsRotatedProportional(const AABB& uvs, uint32_t textureWidth, uint32_t textureHeight)
{
  XR_ASSERT(Sprite, textureWidth > 0);
  XR_ASSERT(Sprite, textureHeight > 0);
  SetUVsRotated(uvs);

  float hw = (uvs.bottom - uvs.top) * .5f * textureWidth;
  float hh = (uvs.right - uvs.left) * .5f * textureHeight;
  SetHalfSize(hw, hh, true);
}

//==============================================================================
void Sprite::Scale( float s )
{
  Scale(s, s);
}

//==============================================================================
void Sprite::Scale( float sx, float sy )
{
  m_halfWidth *= fabsf(sx);
  m_halfHeight *= fabsf(sy);

  for (auto& v: m_vertices)
  {
    v.pos.x *= sx;
    v.pos.y *= sy;
  }
}

//==============================================================================
void Sprite::ScaleX( float sx )
{
  m_halfWidth *= fabsf(sx);

  for (auto& v: m_vertices)
  {
    v.pos.x *= sx;
  }
}

//==============================================================================
void Sprite::ScaleY( float sy )
{
  m_halfHeight *= fabsf(sy);

  for (auto& v: m_vertices)
  {
    v.pos.y *= sy;
  }
}

//==============================================================================
void Sprite::FlipVerticesX()
{
  for (auto& v: m_vertices)
  {
    v.pos.x *= -1.0f;
  }
  m_offset.x *= -1.0f;
}

//==============================================================================
void Sprite::FlipUVsX()
{
  if (m_isUVRotated)
  {
    Vector2 vLeft(m_vertices[Quad::Vertex::NW].uv0);
    Vector2 vRight(m_vertices[Quad::Vertex::NE].uv0);
    std::swap(vLeft.y, vRight.y);
    m_vertices[Quad::Vertex::NW].uv0 = vLeft;
    m_vertices[Quad::Vertex::NE].uv0 = vRight;

    vLeft = m_vertices[Quad::Vertex::SW].uv0;
    vRight = m_vertices[Quad::Vertex::SE].uv0;
    std::swap(vLeft.y, vRight.y);
    m_vertices[Quad::Vertex::SW].uv0 = vLeft;
    m_vertices[Quad::Vertex::SE].uv0 = vRight;
  }
  else
  {
    Vector2 vLeft(m_vertices[Quad::Vertex::NW].uv0);
    Vector2 vRight(m_vertices[Quad::Vertex::NE].uv0);
    std::swap(vLeft.x, vRight.x);
    m_vertices[Quad::Vertex::NW].uv0 = vLeft;
    m_vertices[Quad::Vertex::NE].uv0 = vRight;

    vLeft = m_vertices[Quad::Vertex::SW].uv0;
    vRight = m_vertices[Quad::Vertex::SE].uv0;
    std::swap(vLeft.x, vRight.x);
    m_vertices[Quad::Vertex::SW].uv0 = vLeft;
    m_vertices[Quad::Vertex::SE].uv0 = vRight;
  }
}

//==============================================================================
void Sprite::FlipX()
{
  FlipVerticesX();
  FlipUVsX();
}

//==============================================================================
void Sprite::FlipVerticesY()
{
  for (auto& v : m_vertices)
  {
    v.pos.y *= -1.0f;
  }
  m_offset.y *= -1.0f;
}

//==============================================================================
void Sprite::FlipUVsY()
{
  if (m_isUVRotated)
  {
    Vector2 vLeft(m_vertices[Quad::Vertex::NW].uv0);
    Vector2 vRight(m_vertices[Quad::Vertex::NE].uv0);
    std::swap(vLeft.x, vRight.x);
    m_vertices[Quad::Vertex::NW].uv0 = vLeft;
    m_vertices[Quad::Vertex::NE].uv0 = vRight;

    vLeft = m_vertices[Quad::Vertex::SW].uv0;
    vRight = m_vertices[Quad::Vertex::SE].uv0;
    std::swap(vLeft.x, vRight.x);
    m_vertices[Quad::Vertex::SW].uv0 = vLeft;
    m_vertices[Quad::Vertex::SE].uv0 = vRight;
  }
  else
  {
    Vector2 vLeft(m_vertices[Quad::Vertex::NW].uv0);
    Vector2 vRight(m_vertices[Quad::Vertex::SW].uv0);
    std::swap(vLeft.y, vRight.y);
    m_vertices[Quad::Vertex::NW].uv0 = vLeft;
    m_vertices[Quad::Vertex::SW].uv0 = vRight;

    vLeft = m_vertices[Quad::Vertex::NE].uv0;
    vRight = m_vertices[Quad::Vertex::SE].uv0;
    std::swap(vLeft.y, vRight.y);
    m_vertices[Quad::Vertex::NE].uv0 = vLeft;
    m_vertices[Quad::Vertex::SE].uv0 = vRight;
  }
}

//==============================================================================
void Sprite::FlipY()
{
  FlipVerticesY();
  FlipUVsY();
}

//==============================================================================
Mesh Sprite::CreateMesh() const
{
  return Mesh::Create(XR_ARRAY_SIZE(m_vertices), m_vertices);
}


} // XR

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <algorithm>
#include "XR/utils.hpp"
#include "XR/Sprite.hpp"

namespace XR
{

//==============================================================================
const uint16_t  Sprite::karIndices[] = { 0, 1, 2, 0, 2, 3 };

const AABB Sprite::kWholeTexture = { .0f, .0f, 1.0f, 1.0f };
const AABB Sprite::kNullTexture =  { .0f, .0f, .0f, .0f };

//==============================================================================
void Sprite::Slice(AABB base, uint32_t across, uint32_t down, uint32_t maxSlices,
  Material::Ptr const& material, Sprite* sprites)
{
  XR_ASSERTMSG(Sprite, material, ("Trying to Slice() null material."));
  if (maxSlices <= 0)
  {
    maxSlices = down * across;
  }

  const float leftStart(base.left);
  const float width(base.right - base.left);
  const float height(base.bottom - base.top);
  decltype(maxSlices) slices = 0;
  for (decltype(down) i = 0; i < down && slices < maxSlices; ++i)
  {
    for (decltype(across) j = 0; j < across && slices < maxSlices; ++j)
    {
      sprites->SetMaterial(material);
      sprites->SetUVsProportional(base);

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
void Sprite::CalculateUVs(AABB const & aabb, bool rotate, Vertex verts[kNumVertices])
{
  if(rotate)
  {
    verts[VI_NW].uv0 = Vector2(aabb.right, aabb.top);
    verts[VI_SW].uv0 = Vector2(aabb.left, aabb.top);
    verts[VI_SE].uv0 = Vector2(aabb.left, aabb.bottom);
    verts[VI_NE].uv0 = Vector2(aabb.right, aabb.bottom);
  }
  else
  {
    verts[VI_NW].uv0 = Vector2(aabb.left, aabb.top);
    verts[VI_SW].uv0 = Vector2(aabb.left, aabb.bottom);
    verts[VI_SE].uv0 = Vector2(aabb.right, aabb.bottom);
    verts[VI_NE].uv0 = Vector2(aabb.right, aabb.top);
  }
}

//==============================================================================
bool Sprite::IsUVRotated(Vertex const verts[kNumVertices])
{
  return verts[VI_NW].uv0.y != verts[VI_NE].uv0.y &&
    verts[VI_NW].uv0.x != verts[VI_SW].uv0.x;
}

//==============================================================================
void Sprite::CopyIndicesTo(uint16_t indices[kNumIndices])
{
  XR_ASSERT(Sprite, indices != 0);
  memcpy(indices, karIndices, sizeof(karIndices));
}

//==============================================================================
Sprite::Sprite()
: BasicMesh(),
  m_halfWidth(.0f),
  m_halfHeight(.0f),
  m_isUVRotated(false),
  m_offset(Vector2::Zero())
{
  AllocBuffer(kNumVertices);
}

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
    auto verts = GetVertices();
    verts[VI_NW].pos = Vector3(-hw, -hh, .0f);
    verts[VI_SW].pos = Vector3(-hw, hh, .0f);
    verts[VI_SE].pos = Vector3(hw, hh, .0f);
    verts[VI_NE].pos = Vector3(hw, -hh, .0f);
  }
}

//==============================================================================
void Sprite::AddOffset(float x, float y, bool updateVertices)
{
  if (updateVertices)
  {
    auto verts = GetVertices();
    for (int i = 0; i < kNumVertices; ++i)
    {
      verts[i].pos.x += x;
      verts[i].pos.y += y;
    }
  }

  m_offset.x += x;
  m_offset.y += y;
}

//==============================================================================
void Sprite::Import(Vertex const verts[kNumVertices])
{
  memcpy(GetVertices(), verts, Vertex::kSize * kNumVertices);

  // determine uv rotation
  const bool isUVRotated = IsUVRotated(verts);
  m_isUVRotated = isUVRotated;

  // determine halfsize
  if (isUVRotated)
  {
    m_halfWidth = (verts[VI_SE].pos - verts[VI_NE].pos).Magnitude() * .5f;
    m_halfHeight = (verts[VI_SW].pos - verts[VI_SE].pos).Magnitude() * .5f;
  }
  else
  {
    m_halfWidth = (verts[VI_SE].pos - verts[VI_SW].pos).Magnitude() * .5f;
    m_halfHeight = (verts[VI_NW].pos - verts[VI_SW].pos).Magnitude() * .5f;
  }

  // determine offset
  auto vertsEnd = verts + kNumVertices;
  Vector2 pos(.0f, .0f);
  while (verts != vertsEnd)
  {
    pos += verts->pos.XY();
    ++verts;
  }
  pos *= 1.0f / kNumVertices;
  m_offset = pos;
}

//==============================================================================
void Sprite::SetUVs(const AABB& tc)
{
  m_isUVRotated = false;
  CalculateUVs(tc, false, GetVertices());
}

//==============================================================================
void Sprite::SetUVsProportional(const AABB& tc)
{
  XR_ASSERT(Sprite, m_material);
  XR_ASSERT(Sprite, m_material->GetTexture(0));

  SetUVs(tc);

  float hw = (tc.right - tc.left) * .5f * m_material->GetTexture(0)->GetWidth();
  float hh = (tc.bottom - tc.top) * .5f * m_material->GetTexture(0)->GetHeight();
  SetHalfSize(hw, hh, true);
}

//==============================================================================
void Sprite::SetUVsRotated(const AABB& tc)
{
  m_isUVRotated = true;
  CalculateUVs(tc, true, GetVertices());
}

//==============================================================================
void Sprite::SetUVsRotatedProportional(const AABB& tc)
{
  XR_ASSERT(Sprite, m_material);
  XR_ASSERT(Sprite, m_material->GetTexture(0));

  SetUVsRotated(tc);

  float hw = (tc.bottom - tc.top) * .5f * m_material->GetTexture(0)->GetHeight();
  float hh = (tc.right - tc.left) * .5f * m_material->GetTexture(0)->GetWidth();
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

  auto verts = GetVertices();
  for (int i = 0; i < kNumVertices; ++i)
  {
    verts[i].pos.x *= sx;
    verts[i].pos.y *= sy;
  }
}

//==============================================================================
void Sprite::ScaleX( float sx )
{
  m_halfWidth *= fabsf(sx);

  auto verts = GetVertices();
  for (int i = 0; i < kNumVertices; ++i)
  {
    verts[i].pos.x *= sx;
  }
}

//==============================================================================
void Sprite::ScaleY( float sy )
{
  m_halfHeight *= fabsf(sy);

  auto verts = GetVertices();
  for (int i = 0; i < kNumVertices; ++i)
  {
    verts[i].pos.y *= sy;
  }
}

//==============================================================================
void Sprite::FlipX()
{
  FlipVerticesX();
  FlipUVsX();
}

//==============================================================================
void Sprite::FlipVerticesX()
{
  auto verts = GetVertices();
  for (int i = 0; i < kNumVertices; ++i)
  {
    verts[i].pos.x *= -1.0f;
  }
  m_offset.x *= -1.0f;
}

//==============================================================================
void Sprite::FlipUVsX()
{
  auto verts = GetVertices();
  if (m_isUVRotated)
  {
    Vector2 vLeft(verts[VI_NW].uv0);
    Vector2 vRight(verts[VI_NE].uv0);
    std::swap(vLeft.y, vRight.y);
    verts[VI_NW].uv0 = vLeft;
    verts[VI_NE].uv0 = vRight;

    vLeft = verts[VI_SW].uv0;
    vRight = verts[VI_SE].uv0;
    std::swap(vLeft.y, vRight.y);
    verts[VI_SW].uv0 = vLeft;
    verts[VI_SE].uv0 = vRight;
  }
  else
  {
    Vector2 vLeft(verts[VI_NW].uv0);
    Vector2 vRight(verts[VI_NE].uv0);
    std::swap(vLeft.x, vRight.x);
    verts[VI_NW].uv0 = vLeft;
    verts[VI_NE].uv0 = vRight;

    vLeft = verts[VI_SW].uv0;
    vRight = verts[VI_SE].uv0;
    std::swap(vLeft.x, vRight.x);
    verts[VI_SW].uv0 = vLeft;
    verts[VI_SE].uv0 = vRight;
  }
}

//==============================================================================
void Sprite::FlipY()
{
  FlipVerticesY();
  FlipUVsY();
}

//==============================================================================
void Sprite::FlipVerticesY()
{
  auto verts = GetVertices();
  for (int i = 0; i < kNumVertices; ++i)
  {
    verts[i].pos.y *= -1.0f;
  }
  m_offset.y *= -1.0f;
}

//==============================================================================
void Sprite::FlipUVsY()
{
  auto verts = GetVertices();
  if (m_isUVRotated)
  {
    Vector2 vLeft(verts[VI_NW].uv0);
    Vector2 vRight(verts[VI_NE].uv0);
    std::swap(vLeft.x, vRight.x);
    verts[VI_NW].uv0 = vLeft;
    verts[VI_NE].uv0 = vRight;

    vLeft = verts[VI_SW].uv0;
    vRight = verts[VI_SE].uv0;
    std::swap(vLeft.x, vRight.x);
    verts[VI_SW].uv0 = vLeft;
    verts[VI_SE].uv0 = vRight;
  }
  else
  {
    Vector2 vLeft(verts[VI_NW].uv0);
    Vector2 vRight(verts[VI_SW].uv0);
    std::swap(vLeft.y, vRight.y);
    verts[VI_NW].uv0 = vLeft;
    verts[VI_SW].uv0 = vRight;

    vLeft = verts[VI_NE].uv0;
    vRight = verts[VI_SE].uv0;
    std::swap(vLeft.y, vRight.y);
    verts[VI_NE].uv0 = vLeft;
    verts[VI_SE].uv0 = vRight;
  }
}

} // XR

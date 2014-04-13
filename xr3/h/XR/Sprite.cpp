//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include <algorithm>
#include "utils.hpp"
#include "Sprite.hpp"

namespace XR
{

//==============================================================================
const uint16  Sprite::karIndices[] = { 0, 1, 2, 0, 2, 3 };

const AABB Sprite::kWholeTexture = { .0f, .0f, 1.0f, 1.0f };
const AABB Sprite::kNullTexture =  { .0f, .0f, .0f, .0f };

//==============================================================================
void Sprite::Slice( AABB base, int across, int down, int maxSlices,
  Material* pMaterial, Sprite* parSprites )
{
  XR_ASSERTMSG(Sprite, pMaterial != 0,
    ("Trying to Slice() NULL material."));
  XR_ASSERT(Sprite, across > 0);
  XR_ASSERT(Sprite, down > 0);   
  if(maxSlices <= 0)
  {
    maxSlices = down * across;
  }

  const float leftStart(base.left);
  const float width(base.right - base.left);
  const float height(base.bottom - base.top);
  int slices(0);
  for(int i = 0; i < down && slices < maxSlices; ++i)
  {
    for(int j = 0; j < across && slices < maxSlices; ++j)
    {
      parSprites->SetMaterial(pMaterial);
      parSprites->SetUVsProportional(base);

      base.left = base.right;
      base.right += width;
      ++slices;
      ++parSprites;
    }
    base.left = leftStart;
    base.right = leftStart + width;
    base.top = base.bottom;
    base.bottom += height;
  }
}

//==============================================================================
void Sprite::CopyWholeTextureUVsTo(RenderStream& uvs)
{
  uvs.Set(VI_NW, Vector2(kWholeTexture.left,
    kWholeTexture.top));
  uvs.Set(VI_SW, Vector2(kWholeTexture.left,
    kWholeTexture.bottom));
  uvs.Set(VI_SE, Vector2(kWholeTexture.right,
    kWholeTexture.bottom));
  uvs.Set(VI_NE, Vector2(kWholeTexture.right,
    kWholeTexture.top));
}

//==============================================================================
void Sprite::CopyWholeTextureUVsTo(int offset, RenderStream& uvs)
{
  uvs.Set(offset + VI_NW, Vector2(kWholeTexture.left,
    kWholeTexture.top));
  uvs.Set(offset + VI_SW, Vector2(kWholeTexture.left,
    kWholeTexture.bottom));
  uvs.Set(offset + VI_SE, Vector2(kWholeTexture.right,
    kWholeTexture.bottom));
  uvs.Set(offset + VI_NE, Vector2(kWholeTexture.right,
    kWholeTexture.top));
}

//==============================================================================
void Sprite::CopyIndicesTo(uint16* parInds, int offset)
{
  XR_ASSERT(Sprite, parInds != 0);
  const uint16* i0(karIndices);
  const uint16* i1(i0 + kNumIndices);
  while(i0 != i1)
  {
    *parInds = *i0 + offset;
    ++i0;
    ++parInds;
  }
}

//==============================================================================
Sprite::Sprite()
: BasicMesh(),
  m_isUVRotated(false),
  m_halfWidth(.0f),
  m_halfHeight(.0f)
{
  InitStreams(kNumVertices);
}

//==============================================================================
Sprite::~Sprite()
{}

//==============================================================================
void  Sprite::SetMaterial(Material* pMaterial)
{
  m_pMaterial = pMaterial;
}

//==============================================================================
void Sprite::SetHalfSize( int32 hw, int32 hh, bool calculateVertices)
{
  XR_ASSERT(Sprite, hw >= 0);
  XR_ASSERT(Sprite, hh >= 0);

  m_halfWidth = hw;
  m_halfHeight = hh;

  if(calculateVertices)
  {
    m_vertices.Set(VI_NW, Vector3(-hw, -hh, .0f));
    m_vertices.Set(VI_SW, Vector3(-hw, hh, .0f));
    m_vertices.Set(VI_SE, Vector3(hw, hh, .0f));
    m_vertices.Set(VI_NE, Vector3(hw, -hh, .0f));
  }
}

//==============================================================================
void Sprite::SetUVs(const AABB& tc)
{
  m_isUVRotated = false;
  m_uvs.Set(VI_NW, Vector2(tc.left, tc.top));
  m_uvs.Set(VI_SW, Vector2(tc.left, tc.bottom));
  m_uvs.Set(VI_SE, Vector2(tc.right, tc.bottom));
  m_uvs.Set(VI_NE, Vector2(tc.right, tc.top));
}

//==============================================================================
void Sprite::SetUVsProportional(const AABB& tc)
{
  XR_ASSERT(Sprite, m_pMaterial != 0);
  XR_ASSERT(Sprite, m_pMaterial->GetTexture(0).GetImpl() != 0);

  SetUVs(tc);

  int32 hw = (tc.right - tc.left) * .5f * m_pMaterial->GetTexture(0).GetWidth();
  int32 hh = (tc.bottom - tc.top) * .5f * m_pMaterial->GetTexture(0).GetHeight();
  SetHalfSize(hw, hh, true);
}

//==============================================================================
void Sprite::SetUVsRotated(const AABB& uvs)
{
  m_isUVRotated = true;
  m_uvs.Set(VI_NW, Vector2(uvs.right, uvs.top));
  m_uvs.Set(VI_SW, Vector2(uvs.left, uvs.top));
  m_uvs.Set(VI_SE, Vector2(uvs.left, uvs.bottom));
  m_uvs.Set(VI_NE, Vector2(uvs.right, uvs.bottom));
}

//==============================================================================
void Sprite::SetUVsRotatedProportional(const AABB& uvs)
{
  XR_ASSERT(Sprite, m_pMaterial != 0);
  XR_ASSERT(Sprite, m_pMaterial->GetTexture(0).GetImpl() != 0);

  SetUVsRotated(uvs);

  int32 hw = (uvs.bottom - uvs.top) * .5f * m_pMaterial->GetTexture(0).GetHeight();
  int32 hh = (uvs.right - uvs.left) * .5f * m_pMaterial->GetTexture(0).GetWidth();
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

  for(int i = 0; i < kNumVertices; ++i)
  {
    Vector3 v(m_vertices.GetVector3(i));
    v.x *= sx;
    v.y *= sy;
    m_vertices.Set(i, v);
  }
}

//==============================================================================
void Sprite::ScaleX( float sx )
{
  m_halfWidth *= fabsf(sx);
  m_vertices.ScaleX(sx);
}

//==============================================================================
void Sprite::ScaleY( float sy )
{
  m_halfHeight *= fabsf(sy);
  m_vertices.ScaleY(sy);
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
  m_vertices.ScaleX(-1.0f);
  m_offset.x *= -1.0f;
}

//==============================================================================
void Sprite::FlipUVsX()
{
  if(m_isUVRotated)
  {
    Vector3 vLeft(m_uvs.GetVector3(VI_NW));
    Vector3 vRight(m_uvs.GetVector3(VI_NE));
    std::swap(vLeft.y, vRight.y);
    m_uvs.Set(VI_NW, vLeft);
    m_uvs.Set(VI_NE, vRight);

    vLeft = m_uvs.GetVector3(VI_SW);
    vRight = m_uvs.GetVector3(VI_SE);
    std::swap(vLeft.y, vRight.y);
    m_uvs.Set(VI_SW, vLeft);
    m_uvs.Set(VI_SE, vRight);
  }
  else
  {
    Vector3 vLeft(m_uvs.GetVector3(VI_NW));
    Vector3 vRight(m_uvs.GetVector3(VI_NE));
    std::swap(vLeft.x, vRight.x);
    m_uvs.Set(VI_NW, vLeft);
    m_uvs.Set(VI_NE, vRight);

    vLeft = m_uvs.GetVector3(VI_SW);
    vRight = m_uvs.GetVector3(VI_SE);
    std::swap(vLeft.x, vRight.x);
    m_uvs.  Set(VI_SW, vLeft);
    m_uvs.Set(VI_SE, vRight);
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
  m_vertices.ScaleY(-1.0f);
  m_offset.y *= -1.0f;
}

//==============================================================================
void Sprite::FlipUVsY()
{
  if(m_isUVRotated)
  {
    Vector3 vLeft(m_uvs.GetVector3(VI_NW));
    Vector3 vRight(m_uvs.GetVector3(VI_NE));
    std::swap(vLeft.x, vRight.x);
    m_uvs.Set(VI_NW, vLeft);
    m_uvs.Set(VI_NE, vRight);

    vLeft = m_uvs.GetVector3(VI_SW);
    vRight = m_uvs.GetVector3(VI_SE);
    std::swap(vLeft.x, vRight.x);
    m_uvs.Set(VI_SW, vLeft);
    m_uvs.Set(VI_SE, vRight);
  }
  else
  {
    Vector3 vLeft(m_uvs.GetVector3(VI_NW));
    Vector3 vRight(m_uvs.GetVector3(VI_SW));
    std::swap(vLeft.y, vRight.y);
    m_uvs.Set(VI_NW, vLeft);
    m_uvs.Set(VI_SW, vRight);

    vLeft = m_uvs.GetVector3(VI_NE);
    vRight = m_uvs.GetVector3(VI_SE);
    std::swap(vLeft.y, vRight.y);
    m_uvs.Set(VI_NE, vLeft);
    m_uvs.Set(VI_SE, vRight);
  }
}

//==============================================================================
void Sprite::OffsetVertices( float x, float y)
{
  m_vertices.Translate(Vector3(x, y, .0f));
  m_offset.x += x;
  m_offset.y += y;
}

//==============================================================================
void  Sprite::RenderRaw()
{
  // the material is already set
  Renderer::SetUVStream(m_uvs, 0);
  Renderer::SetVertStream(m_vertices);
  Renderer::DrawPrims(PRIM_TRI_LIST, karIndices, kNumIndices);
}

} // XR

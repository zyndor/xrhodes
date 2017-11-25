//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_SPRITE_HPP
#define XR_SPRITE_HPP

#include "Renderer.hpp"
#include "AABB.hpp"
#include "BasicMesh.hpp"
#include "XR/fundamentals.hpp"

// TODO: Remove these includes.
#include "XR/ResManager.hpp"

namespace XR
{

//==============================================================================
class Sprite: public  BasicMesh
{
  XR_MANAGED_DECL(Sprite)

public:
  // typedef
  enum  VertexId
  {
    VI_NW,
    VI_SW,
    VI_SE,
    VI_NE,
    kNumVertices
  };

  // static
  static const int32_t  kNumIndices = 6;
  static const uint16_t karIndices[kNumIndices];
  
  static const int32_t  kUVsSize = kNumVertices * sizeof(Vector2);
  static const int32_t  kVertsSize = kNumVertices * sizeof(Vector3);
  
  static const AABB   kWholeTexture;
  static const AABB   kNullTexture;

  ///@brief Using @a base as the UVs of the initial sprite on @a pMaterial,
  /// creates @a across * @a down (but no more than @a maxSlices) Sprites in @a
  /// parSprites, shifting the AABB across and down.
  ///@note  @a parSprites must have space for at least @a maxSlices Sprites.
  static void Slice(AABB base, int across, int down, int maxSlices,
    Material::Ptr const& material, Sprite* sprites);

  static FloatBuffer*   CopyWholeTextureUVs();
  static void           CopyWholeTextureUVsTo(FloatBuffer& uvs);
  static void           CopyWholeTextureUVsTo(size_t offset, FloatBuffer& uvs);

  static void           CopyIndicesTo(uint16_t* indices, size_t offset);

  // structors
  Sprite();
  ~Sprite();

  // general
  ///@return  The material the Sprite is based on.
  Material::Ptr GetMaterial() const;

  ///@return Whether the Sprite is created from 90 degrees clockwise rotated
  /// UVs. (Texture Packer rotated="y" does this.)
  bool        IsUVRotated() const;

  ///@return  The half width of the Sprite.
  ///@note    This includes any padding.
  float       GetHalfWidth() const;

  ///@return  The half height of the Sprite.
  ///@note    This includes any padding.
  float       GetHalfHeight() const;
  
  ///@return  The distance between the top left and top right vertices of the
  /// quad.
  float       GetQuadWidth() const;

  ///@return  The distance between the top left and bottom left vertices of the
  /// quad.
  float       GetQuadHeight() const;
  
  float       GetLeftPadding() const;
  float       GetTopPadding() const;
  float       GetRightPadding() const;
  float       GetBottomPadding() const;

  ///@return  The offsetting that's been applied to the vertices so far.
  const Vector2& GetOffset() const;

  ///@return  The array of UVs that make up the Sprite.
  const FloatBuffer& GetUVs() const;
  
  ///@return  The array of vertices that make up the Sprite.
  ///@return  An IW_GX_ALLOCated copy of the UVs' array.
  FloatBuffer* CopyUVs() const;

  ///@return  An IW_GX_ALLOCated copy of the vertices' array.
  FloatBuffer* CopyVertices() const;
  const FloatBuffer& GetVertices() const;

  ///@brief Copies the UVs to the supplied array.
  void  CopyUVsTo(size_t offset, FloatBuffer& uvs) const;

  ///@brief Copies the vertices to the supplied array.
  void  CopyVerticesTo(size_t offset, FloatBuffer& verts) const;

  void  SetHalfSize(int32_t hw, int32_t hh, bool calculateVertices = true);

  void  SetUVs(const AABB& uvs);
  void  SetUVsProportional(const AABB& uvs);

  // from a rotated definition (90 degs clockwise)
  void  SetUVsRotated(const AABB& uvs);
  void  SetUVsRotatedProportional(const AABB& uvs);

  void  SetMaterial(Material::Ptr const& pMaterial);

  void  Scale(float s);
  void  Scale(float sx, float sy);
  void  ScaleX(float sx);
  void  ScaleY(float sy);

  void  FlipVerticesX();
  void  FlipUVsX();
  void  FlipX();
  
  void  FlipVerticesY();
  void  FlipUVsY();
  void  FlipY();

  void  OffsetVertices(float x, float y);
  void  OffsetVertices(const Vector2& offset);

  virtual void  Render();
  void          RenderRaw();

protected:
  // data
  float         m_halfWidth;
  float         m_halfHeight;
  bool          m_isUVRotated;

  Vector2       m_offset;
};

//==============================================================================
//implementation
//==============================================================================
inline
FloatBuffer* Sprite::CopyWholeTextureUVs()
{
  FloatBuffer* pUVs(Renderer::AllocBuffer(sizeof(Vector2), kNumVertices));
  CopyWholeTextureUVsTo(0, *pUVs);
  return pUVs;
}

//==============================================================================
inline
bool Sprite::IsUVRotated() const
{
  return m_isUVRotated;
}

//==============================================================================
inline
void  Sprite::Render()
{
  m_material->Apply();
  RenderRaw();
}

//==============================================================================
inline
float Sprite::GetHalfWidth() const
{
  return m_halfWidth;
}

//==============================================================================
inline
float Sprite::GetHalfHeight() const
{
  return m_halfHeight;
}

//==============================================================================
inline
float Sprite::GetQuadWidth() const
{
  return m_vertices.Get<Vector3>(VI_NE).x - m_vertices.Get<Vector3>(VI_NW).x;
}

//==============================================================================
inline
float Sprite::GetQuadHeight() const
{
  return m_vertices.Get<Vector3>(VI_SW).y - m_vertices.Get<Vector3>(VI_NW).y;
}

//==============================================================================
inline
float Sprite::GetLeftPadding() const
{
  return m_halfWidth + m_vertices.Get<Vector3>(VI_NW).x;
}
  
//==============================================================================
inline
float Sprite::GetTopPadding() const
{
  return m_halfHeight + m_vertices.Get<Vector3>(VI_NW).y;
}

//==============================================================================
inline
float Sprite::GetRightPadding() const
{
  return m_halfWidth - m_vertices.Get<Vector3>(VI_NE).x;
}

//==============================================================================
inline
float Sprite::GetBottomPadding() const
{
  return m_halfHeight - m_vertices.Get<Vector3>(VI_SW).y;
}
  
//==============================================================================
inline
const FloatBuffer& Sprite::GetUVs() const
{
  return m_uvs;
}

//==============================================================================
inline
const FloatBuffer& Sprite::GetVertices() const
{
  return m_vertices;
}

//==============================================================================
inline
Material::Ptr Sprite::GetMaterial() const
{
  return m_material;
}
  
//==============================================================================
inline
const Vector2&  Sprite::GetOffset() const
{
  return m_offset;
}

//==============================================================================
inline
FloatBuffer* Sprite::CopyUVs() const
{
  FloatBuffer* pFbUVs(Renderer::AllocBuffer(sizeof(Vector2), kNumVertices));
  CopyUVsTo(0, *pFbUVs);
  return pFbUVs;
}

//==============================================================================
inline
FloatBuffer* Sprite::CopyVertices() const
{
  FloatBuffer* pFbVerts(Renderer::AllocBuffer(sizeof(Vector2), kNumVertices));
  CopyVerticesTo(0, *pFbVerts);
  return pFbVerts;
}

//==============================================================================
inline
void Sprite::CopyUVsTo(size_t offset, FloatBuffer& uvs) const
{
  XR_ASSERT(Sprite, uvs.GetElementSizeBytes() == sizeof(Vector2));
  XR_ASSERT(Sprite, uvs.GetNumElements() >= offset + kNumVertices);
  uvs.Set<Vector2>(kNumVertices, m_uvs.Get<Vector2>(), offset);
}

//==============================================================================
inline
void Sprite::CopyVerticesTo(size_t offset, FloatBuffer& verts) const
{
  XR_ASSERT(Sprite, verts.GetElementSizeBytes() == sizeof(Vector3));
  XR_ASSERT(Sprite, verts.GetNumElements() >= offset + kNumVertices);
  verts.Set<Vector3>(kNumVertices, m_uvs.Get<Vector3>());
}

} // XR

#endif // XR_SPRITE_HPP

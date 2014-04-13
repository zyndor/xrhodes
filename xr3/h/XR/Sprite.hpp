//
// Nuclear Heart Games
// XRhodes
//
// Sprite.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    12/01/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_SPRITE_HPP
#define XR_SPRITE_HPP

#include "Renderer.hpp"
#include "AABB.hpp"
#include "BasicMesh.hpp"

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
  static const int32  kNumIndices = 6;
  static const uint16 karIndices[kNumIndices];
  
  static const int32  kUVsSize = kNumVertices * sizeof(Vector2);
  static const int32  kVertsSize = kNumVertices * sizeof(Vector3);
  
  static const AABB   kWholeTexture;
  static const AABB   kNullTexture;

  ///@brief Using @a base as the UVs of the initial sprite on @a pMaterial,
  /// creates @a across * @a down (but no more than @a maxSlices) Sprites in @a
  /// parSprites, shifting the AABB across and down.
  ///@note  @a parSprites must have space for at least @a maxSlices Sprites.
  static void Slice(AABB base, int across, int down, int maxSlices,
    Material* pMaterial, Sprite* parSprites);

  static RenderStream*  CopyWholeTextureUVs();
  static void           CopyWholeTextureUVsTo(RenderStream& uvs);
  static void           CopyWholeTextureUVsTo(int offset, RenderStream& uvs);

  static void           CopyIndicesTo(uint16* parInds, int offset);

  // structors
  Sprite();
  ~Sprite();

  // general
  ///@return  The material the Sprite is based on.
  Material*   GetMaterial() const;

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
  const RenderStream& GetUVs() const;
  
  ///@return  The array of vertices that make up the Sprite.
  const RenderStream& GetVertices() const;

  ///@return  An IW_GX_ALLOCated copy of the UVs' array.
  RenderStream* CopyUVs() const;

  ///@return  An IW_GX_ALLOCated copy of the vertices' array.
  RenderStream* CopyVertices() const;

  ///@brief Copies the UVs to the supplied array.
  void  CopyUVsTo(int offset, RenderStream& uvs) const;

  ///@brief Copies the vertices to the supplied array.
  void  CopyVerticesTo(int offset, RenderStream& verts) const;

  void  SetHalfSize(int32 hw, int32 hh, bool calculateVertices = true);

  void  SetUVs(const AABB& uvs);
  void  SetUVsProportional(const AABB& uvs);

  // from a rotated definition (90 degs clockwise)
  void  SetUVsRotated(const AABB& uvs);
  void  SetUVsRotatedProportional(const AABB& uvs);

  void  SetMaterial(Material* pMaterial);

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
inline
RenderStream* Sprite::CopyWholeTextureUVs()
{
  RenderStream* pUVs(Renderer::AllocStream(RenderStream::F_VECTOR2, kNumVertices));
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
  Renderer::SetMaterial(m_pMaterial);
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
  return m_vertices.GetVector2(VI_NE).x - m_vertices.GetVector2(VI_NW).x;
}

//==============================================================================
inline
float Sprite::GetQuadHeight() const
{
  return m_vertices.GetVector2(VI_SW).y - m_vertices.GetVector2(VI_NW).y;
}

//==============================================================================
inline
float Sprite::GetLeftPadding() const
{
  return m_halfWidth + m_vertices.GetVector3(VI_NW).x;
}
  
//==============================================================================
inline
float Sprite::GetTopPadding() const
{
  return m_halfHeight + m_vertices.GetVector3(VI_NW).y;
}

//==============================================================================
inline
float Sprite::GetRightPadding() const
{
  return m_halfWidth - m_vertices.GetVector3(VI_NE).x;
}

//==============================================================================
inline
float Sprite::GetBottomPadding() const
{
  return m_halfHeight - m_vertices.GetVector3(VI_SW).y;
}
  
//==============================================================================
inline
const RenderStream& Sprite::GetUVs() const
{
  return m_uvs;
}

//==============================================================================
inline
const RenderStream& Sprite::GetVertices() const
{
  return m_vertices;
}

//==============================================================================
inline
Material* Sprite::GetMaterial() const
{
  return m_pMaterial;
}
  
//==============================================================================
inline
const Vector2&  Sprite::GetOffset() const
{
  return m_offset;
}

//==============================================================================
inline
RenderStream* Sprite::CopyUVs() const
{
  RenderStream* pRsUVs(Renderer::AllocStream(RenderStream::F_VECTOR2, kNumVertices));
  CopyUVsTo(0, *pRsUVs);
  return pRsUVs;
}

//==============================================================================
inline
RenderStream* Sprite::CopyVertices() const
{
  RenderStream* pRsVerts(Renderer::AllocStream(RenderStream::F_VECTOR3, kNumVertices));
  CopyVerticesTo(0, *pRsVerts);
  return pRsVerts;
}

//==============================================================================
inline
void Sprite::CopyUVsTo(int offset, RenderStream& uvs) const
{
  XR_ASSERT(Sprite, uvs.GetFormat() == RenderStream::F_VECTOR2);
  uvs.Copy(m_uvs, 0, Sprite::kNumVertices, offset);
}

//==============================================================================
inline
void Sprite::CopyVerticesTo(int offset, RenderStream& verts) const
{
  XR_ASSERT(Sprite, verts.GetFormat() == RenderStream::F_VECTOR3);
  verts.Copy(m_vertices, 0, Sprite::kNumVertices, offset);
}

} // XR

#endif // XR_SPRITE_HPP

#ifndef XR_SPRITE_HPP
#define XR_SPRITE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "AABB.hpp"
#include "BasicMesh.hpp"
#include "Vertex.hpp"
#include "XR/fundamentals.hpp"

// TODO: Remove these includes.
#include "XR/ResManager.hpp"

namespace XR
{

//==============================================================================
using SpriteVertexFormat = Vertex::Format<Vertex::Pos<Vector3>, Vertex::UV0<Vector2>>;

class Sprite: public BasicMesh<SpriteVertexFormat>
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

  using Vertex = SpriteVertexFormat;

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
  ///@note  @a parSprites must have space for at least @a maxSlices or
  /// @a across x @a down Sprites, whichever is smaller.
  static void Slice(AABB base, uint32_t across, uint32_t down, uint32_t maxSlices,
    Material::Ptr const& material, Sprite* sprites);

  ///@brief Sets the UVs on @a verts from the given @a aabb, based on whether
  /// it is @a rotated or not. UV rotation means a sprite rotated 90 degrees
  /// clockwise, which we need to offset.
  static void CalculateUVs(AABB const& aabb, bool rotate, Vertex verts[kNumVertices]);

  ///@brief Determines whether the given vertices have uv rotation.
  static bool IsUVRotated(Vertex const verts[kNumVertices]);

  static void  CopyIndicesTo(uint16_t indices[kNumIndices]);

  // structors
  Sprite();
  ~Sprite();

  // general
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

  ///@brief Copies the UVs to the supplied array.
  template <typename VertexFormat>
  void  CopyUVsTo(VertexFormat* verts) const;

  ///@brief Copies the positions to the supplied array.
  template <typename VertexFormat>
  void  CopyPositionsTo(VertexFormat* verts) const;

  ///@brief Copies the vertices to the supplied array.
  template <typename VertexFormat>
  void  CopyVerticesTo(VertexFormat* verts) const;

  ///@brief Sets the halfSize of the sprite, which may be used for layouting.
  /// Note that this may be different from what the vertex data defines, if
  /// the Sprite had fully transparent areas trimmed away. (Which a non-zero
  /// offset will suggest.)
  void  SetHalfSize(float hw, float hh, bool calculateVertices);

  ///@brief Adds the given amounts to the offset of the sprite, optionally
  /// updating (translating) vertex data.
  void  AddOffset(float x, float y, bool updateVertices);

  ///@brief Copies vertex data, determines UV rotation, halfSize and offset
  /// (for padding).
  void  Import(Vertex const verts[kNumVertices]);

  void  SetUVs(const AABB& uvs);
  void  SetUVsProportional(const AABB& uvs);

  // from a rotated definition (90 degs clockwise)
  void  SetUVsRotated(const AABB& uvs);
  void  SetUVsRotatedProportional(const AABB& uvs);

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

protected:
  // data
  float m_halfWidth;
  float m_halfHeight;
  bool  m_isUVRotated;

  Vector2 m_offset;
};

//==============================================================================
//implementation
//==============================================================================
inline
bool Sprite::IsUVRotated() const
{
  return m_isUVRotated;
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
  auto verts = GetVertices();
  return verts[VI_NE].pos.x - verts[VI_NW].pos.x;
}

//==============================================================================
inline
float Sprite::GetQuadHeight() const
{
  auto verts = GetVertices();
  return verts[VI_SW].pos.y - verts[VI_NW].pos.y;
}

//==============================================================================
inline
float Sprite::GetLeftPadding() const
{
  return m_halfWidth + GetVertices()[VI_NW].pos.x;
}

//==============================================================================
inline
float Sprite::GetTopPadding() const
{
  return m_halfHeight + GetVertices()[VI_NW].pos.y;
}

//==============================================================================
inline
float Sprite::GetRightPadding() const
{
  return m_halfWidth - GetVertices()[VI_NE].pos.x;
}

//==============================================================================
inline
float Sprite::GetBottomPadding() const
{
  return m_halfHeight - GetVertices()[VI_SW].pos.y;
}

//==============================================================================
inline
const Vector2&  Sprite::GetOffset() const
{
  return m_offset;
}

//==============================================================================
template <typename VertexFormat>
inline
void Sprite::CopyUVsTo(VertexFormat* verts) const
{
  auto vertsEnd = verts + kNumVertices;
  auto source = GetVertices();
  while(verts != vertsEnd)
  {
    verts->uv0 = source->uv0;
    ++verts;
    ++source;
  }
}

//==============================================================================
template <typename VertexFormat>
inline
void Sprite::CopyPositionsTo(VertexFormat* verts) const
{
  auto vertsEnd = verts + kNumVertices;
  auto source = GetVertices();
  while (verts != vertsEnd)
  {
    verts->pos = source->pos;
    ++verts;
    ++source;
  }
}

//==============================================================================
template <typename VertexFormat>
inline
void Sprite::CopyVerticesTo(VertexFormat* verts) const
{
  auto vertsEnd = verts + kNumVertices;
  auto source = GetVertices();
  while (verts != vertsEnd)
  {
    *verts = *source;
    ++verts;
    ++source;
  }
}

} // XR

#endif // XR_SPRITE_HPP

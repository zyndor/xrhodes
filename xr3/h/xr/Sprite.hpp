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
#include "Vertex.hpp"
#include "Quad.hpp"

namespace xr
{

struct Mesh;

//==============================================================================
///@brief A textured quad with facilities for:
/// - setting up from upright as well as rotated definitions;
/// - offsetting;
/// - vertex manipulation;
/// - Mesh creation;
class Sprite
{
public:
  // typedef
  using Vertex = Vertex::Format<Vertex::Pos<Vector3>, Vertex::UV0<Vector2>>;


  // static
  static const uint32_t  kUVsSize = Quad::Vertex::kCount * sizeof(Vector2);
  static const uint32_t  kVertsSize = Quad::Vertex::kCount * sizeof(Vector3);

  static const AABB   kWholeTexture;
  static const AABB   kNullTexture;

  ///@brief Using @a base as the UVs of the initial sprite over a @a textureWidth
  /// by @a textureHeight texture, it creates @a across * @a down (but no more
  /// than @a maxSlices) Sprites in @a sprites, shifting the AABB across and down.
  ///@note  @a sprites must have space for at least @a maxSlices or @a across x
  /// @a down Sprites, whichever is smaller.
  static void Slice(AABB base, uint32_t textureWidth, uint32_t textureHeight,
    uint32_t across, uint32_t down, uint32_t maxSlices, Sprite* sprites);

  ///@brief Sets the UVs on @a verts from the given @a aabb, based on whether
  /// it is @a rotated or not. UV rotation means a sprite rotated 90 degrees
  /// clockwise, which we need to offset.
  static void CalculateUVs(AABB const& aabb, bool rotate, Vertex verts[Quad::Vertex::kCount]);

  ///@brief Determines whether the given vertices have uv rotation.
  static bool IsUVRotated(Vertex const verts[Quad::Vertex::kCount]);

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

  ///@return  The distance between the left side of the sprite and its geometry.
  float       GetLeftPadding() const;

  ///@return  The distance between the top of the sprite and its geometry.
  float       GetTopPadding() const;

  ///@return  The distance between the right side of the sprite and its geometry.
  float       GetRightPadding() const;

  ///@return  The distance between the bottom of the sprite and its geometry.
  float       GetBottomPadding() const;

  ///@return  The offsetting that's been applied to the vertices so far.
  const Vector2& GetOffset() const;

  ///@return  The vertex data of the sprite.
  Vertex const* GetVertices() const;

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
  /// the Sprite had areas (of e.g. full transparency) trimmed away. (Which a
  /// non-zero offset will suggest.)
  void  SetHalfSize(float hw, float hh, bool calculateVertices);

  ///@brief Adds the given amounts to the offset of the sprite, optionally
  /// updating (translating) vertex data.
  void  AddOffset(float x, float y, bool updateVertices);

  ///@brief Sets the UVs of the Sprite based on @a uvs.
  void  SetUVs(const AABB& uvs);

  ///@brief Sets the UVs of the Sprite based on @a uvs, and updates the vertex
  /// positions based on @a textureWidth and @a textureHeight (1 unit - 1 texel).
  void  SetUVsProportional(const AABB& uvs, uint32_t textureWidth, uint32_t textureHeight);

  ///@brief Sets the UVs of the Sprite based on @a uvs, assuming a rotated
  /// definition (i.e. 90 degrees clockwise - right is top, top is left, etc.).
  void  SetUVsRotated(const AABB& uvs);
  void  SetUVsRotatedProportional(const AABB& uvs, uint32_t textureWidth, uint32_t textureHeight);

  ///@brief Scales both sprite size and vertex positions, by @a s.
  void  Scale(float s);

  ///@brief Scales both sprite size and vertex positions, by @a sx in x and by
  /// @a sy in y.
  void  Scale(float sx, float sy);

  ///@brief Scales both sprite size and vertex positions, in x only, by @a sx.
  void  ScaleX(float sx);

  ///@brief Scales both sprite size and vertex positions, in y only, by @a sy.
  void  ScaleY(float sy);

  ///@brief Flips the x component of vertex positions and offset.
  void  FlipVerticesX();

  ///@brief Flips the left and right uvs.
  void  FlipUVsX();

  ///@brief Performs a combination of FlipVerticesX() and FlipUVsX().
  void  FlipX();

  ///@brief Flips the y component of vertex positions and offset.
  void  FlipVerticesY();

  ///@brief Flips the bottom and top uvs.
  void  FlipUVsY();

  ///@brief Performs a combination of FlipVerticesY() and FlipUVsY().
  void  FlipY();

  ///@brief Creates a mesh with the vertex data of the Sprite.
  Mesh  CreateMesh() const;

protected:
  // data
  Vertex m_vertices[Quad::Vertex::kCount];
  Vector2 m_halfSize;
  Vector2 m_offset;
};

//==============================================================================
//implementation
//==============================================================================
inline
bool Sprite::IsUVRotated() const
{
  return IsUVRotated(m_vertices);
}

//==============================================================================
inline
float Sprite::GetHalfWidth() const
{
  return m_halfSize.x;
}

//==============================================================================
inline
float Sprite::GetHalfHeight() const
{
  return m_halfSize.y;
}

//==============================================================================
inline
float Sprite::GetQuadWidth() const
{
  auto verts = GetVertices();
  return verts[Quad::Vertex::NE].pos.x - verts[Quad::Vertex::NW].pos.x;
}

//==============================================================================
inline
float Sprite::GetQuadHeight() const
{
  auto verts = GetVertices();
  return verts[Quad::Vertex::SW].pos.y - verts[Quad::Vertex::NW].pos.y;
}

//==============================================================================
inline
float Sprite::GetLeftPadding() const
{
  return m_halfSize.x + GetVertices()[Quad::Vertex::NW].pos.x;
}

//==============================================================================
inline
float Sprite::GetTopPadding() const
{
  return m_halfSize.y - GetVertices()[Quad::Vertex::NW].pos.y;
}

//==============================================================================
inline
float Sprite::GetRightPadding() const
{
  return m_halfSize.x - GetVertices()[Quad::Vertex::NE].pos.x;
}

//==============================================================================
inline
float Sprite::GetBottomPadding() const
{
  return m_halfSize.y + GetVertices()[Quad::Vertex::SW].pos.y;
}

//==============================================================================
inline
const Vector2&  Sprite::GetOffset() const
{
  return m_offset;
}

//==============================================================================
inline
Sprite::Vertex const* Sprite::GetVertices() const
{
  return m_vertices;
}

//==============================================================================
template <typename VertexFormat>
inline
void Sprite::CopyUVsTo(VertexFormat* verts) const
{
  auto vertsEnd = verts + Quad::Vertex::kCount;
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
  auto vertsEnd = verts + Quad::Vertex::kCount;
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
  auto vertsEnd = verts + Quad::Vertex::kCount;
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

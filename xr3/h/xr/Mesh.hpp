#ifndef XR_BASICMESH_HPP
#define XR_BASICMESH_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "meshutil.hpp"
#include "FloatBuffer.hpp"
#include "VertexFormats.hpp"
#include "xr/debug.hpp"
#include <stdint.h>

namespace xr {

//==============================================================================
class BasicMesh
{
public:
  // structors
  BasicMesh(uint32_t vertexSize, uint32_t numVertices = 0);
  virtual ~BasicMesh();

  // general
  ///@return The vertex data as a Buffer.
  Buffer GetVertexBuffer() const;

  ///@brief Renders the mesh as the given primitive type.
  virtual void Render(Primitive primitive) const;

  ///@brief Renders @a count vertices from @a offset, of the mesh, as the given
  /// primitive type.
  virtual void Render(Primitive primitive, uint32_t offset, uint32_t count) const;

protected:
  // data
  Gfx::VertexBufferHandle m_vbo;
  FloatBuffer m_vertices;
};

//==============================================================================
class IndexMesh: public BasicMesh
{
public:
  // types
  using IndexArray = std::vector<uint16_t>;

  // structors
  IndexMesh(uint32_t vertexSize, uint32_t numVertices = 0);
  ~IndexMesh();

  // general
  ///@return The vector of indices.
  IndexArray& GetIndices();

  ///@return The const vector of indices.
  const IndexArray& GetIndices() const;

  ///@return The index data as a Buffer.
  Buffer GetIndexBuffer() const;

  ///@return Sets the indices to be a repeating pattern of @a numInds indices,
  /// copied @a numSets times, offset each time by the largest index + 1. i.e.
  /// SetIndexPattern({ 0, 2, 1 }, 3, 2) will create { 0, 2, 1, 3, 5, 4 }.
  void  SetIndexPattern(const uint16_t* indices, uint32_t numInds, uint32_t numSets);

  ///@return Sets the indices to be a repeating pattern of @a numInds indices,
  /// copied @a numSets times, offset each time by @a offset. i.e.
  /// SetIndexPattern({ 0, 2, 1 }, 3, 10, 2) will create { 0, 2, 1, 10, 12, 11 }.
  void  SetIndexPattern(const uint16_t* indices, uint32_t numInds, uint16_t offset,
    uint32_t numSets);

  ///@return Creates the Index Buffer Object based on the current index data,
  /// deleting the previous one, if any.
  void CreateIbo(uint32_t flags = Gfx::F_BUFFER_NONE);

  ///@brief Renders the mesh using the index information, without setting the
  /// material.
  void Render(Primitive primitive) const;

  ///@brief Renders the mesh using @a count indices starting from @a offset,
  /// of the index information, without setting the material.
  void Render(Primitive primitive, uint32_t offset, uint32_t count) const;

protected:
  // data
  Gfx::IndexBufferHandle m_ibo;
  IndexArray  m_indices;
};

//==============================================================================
template <class VertexFormat, class Base = BasicMesh>
class Mesh: public Base
{
  static_assert(std::is_base_of<BasicMesh, Base>::value, "Base must be, or derive  from, BasicMesh.");

public:
  // types
  using PosType = decltype(VertexFormat::pos);

  // structors
  Mesh(uint32_t numVertices = 0);

  // general
  ///@brief Allocates buffer for @a numVertices vertices.
  void AllocBuffer(uint32_t numVertices);

  ///@return A pointer to the buffer data.
  VertexFormat* GetVertices();

  ///@return A pointer to the const buffer data.
  const VertexFormat* GetVertices() const;

  ///@brief Creates a VBO using its vertex data, and storing the resulting
  /// handle internally (the previously held handle will be deleted).
  void CreateVbo(uint32_t flags = Gfx::F_BUFFER_NONE);

  ///@brief Calculates the centre of mass of the mesh.
  PosType CalculateCentre() const;

  ///@brief Finds the distance of the farthest vertex of the mesh, from the origin.
  float CalculateRadius() const;

  ///@brief Finds the axis aligned bounding box of the mesh and writes its
  /// minimum and maximum to @a minOut and @a maxOut.
  void CalculateExtents(PosType& minOut, PosType& maxOut) const;
};

//==============================================================================
// implementation
//==============================================================================
inline
Buffer BasicMesh::GetVertexBuffer() const
{
  return Buffer{ m_vertices.GetElementSizeBytes() * m_vertices.GetNumElements(),
    reinterpret_cast<uint8_t const*>(m_vertices.GetRaw()) };
}

//==============================================================================
inline
void BasicMesh::Render(Primitive primitive) const
{
  Render(primitive, 0, m_vertices.GetNumElements());
}

//==============================================================================
inline
void BasicMesh::Render(Primitive primitive, uint32_t offset, uint32_t count) const
{
  XR_ASSERT(Mesh, m_vbo.IsValid());
  Gfx::Draw(m_vbo, primitive, offset, count);
}

//==============================================================================
inline
IndexMesh::IndexArray& IndexMesh::GetIndices()
{
  return m_indices;
}

//==============================================================================
inline
const IndexMesh::IndexArray& IndexMesh::GetIndices() const
{
  return m_indices;
}

//==============================================================================
inline
Buffer IndexMesh::GetIndexBuffer() const
{
  return { m_indices.size() * sizeof(decltype(m_indices)::value_type),
    reinterpret_cast<const uint8_t*>(m_indices.data()) };
}

//==============================================================================
inline
void IndexMesh::Render(Primitive primitive) const
{
  Render(primitive, 0, uint32_t(m_indices.size()));
}

//==============================================================================
inline
void IndexMesh::Render(Primitive primitive, uint32_t offset, uint32_t count) const
{
  XR_ASSERT(Mesh, m_vbo.IsValid());
  XR_ASSERT(Mesh, m_ibo.IsValid());
  Gfx::Draw(m_vbo, m_ibo, primitive, offset, count);
}

//==============================================================================
template <class VertexFormat, class Base>
Mesh<VertexFormat, Base>::Mesh(uint32_t numVertices)
: Base(VertexFormat::kSize, numVertices)
{}

//==============================================================================
template <class VertexFormat, class Base>
void  Mesh<VertexFormat, Base>::AllocBuffer(uint32_t numVertices)
{
  XR_ASSERT(Mesh, numVertices >= 0);
  Base::m_vertices.SetBuffer(VertexFormat::kSize, numVertices);
}

//==============================================================================
template<class VertexFormat, class Base>
inline
VertexFormat* Mesh<VertexFormat, Base>::GetVertices()
{
  return Base::m_vertices.template Get<VertexFormat>();
}

//==============================================================================
template<class VertexFormat, class Base>
inline
const VertexFormat* Mesh<VertexFormat, Base>::GetVertices() const
{
  return Base::m_vertices.template Get<VertexFormat>();
}

//==============================================================================
template <class VertexFormat, class Base>
inline
void Mesh<VertexFormat, Base>::CreateVbo(uint32_t flags)
{
  Gfx::Release(Base::m_vbo);
  XR_ASSERT(Mesh, Base::m_vertices.GetRaw() != nullptr);
  Base::m_vbo = Gfx::CreateVertexBuffer(Vertex::Formats::GetHandle<VertexFormat>(),
    Base::GetVertexBuffer(), flags);
}

//==============================================================================
template <class VertexFormat, class Base>
inline
typename Mesh<VertexFormat, Base>::PosType
  Mesh<VertexFormat, Base>::CalculateCentre() const
{
  return meshutil::CalculateCentre(Base::m_vertices.GetNumElements(), GetVertices());
}

//==============================================================================
template <class VertexFormat, class Base>
inline
float Mesh<VertexFormat, Base>::CalculateRadius() const
{
  return meshutil::CalculateRadius(Base::m_vertices.GetNumElements(), GetVertices());
}

//==============================================================================
template <class VertexFormat, class Base>
inline
void  Mesh<VertexFormat, Base>::CalculateExtents(PosType& outMin, PosType& outMax) const
{
  meshutil::CalculateExtents(Base::m_vertices.GetNumElements(), GetVertices(), outMin, outMax);
}

} // xr

#endif // XR_BASICMESH_HPP
#ifndef XR_BASICMESH_HPP
#define XR_BASICMESH_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "FloatBuffer.hpp"
#include "VertexFormats.hpp"
#include <stdint.h>

namespace XR {

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
  void  SetIndexPattern(const uint16_t* pInds, uint32_t numInds, uint32_t numSets);

  ///@return Sets the indices to be a repeating pattern of @a numInds indices,
  /// copied @a numSets times, offset each time by @a offset. i.e.
  /// SetIndexPattern({ 0, 2, 1 }, 3, 10, 2) will create { 0, 2, 1, 10, 12, 11 }.
  void  SetIndexPattern(const uint16_t* pInds, uint32_t numInds, uint16_t offset,
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
  m_vertices.SetBuffer(VertexFormat::kSize, numVertices);
}

//==============================================================================
template<class VertexFormat, class Base>
inline
VertexFormat* Mesh<VertexFormat, Base>::GetVertices()
{
  return m_vertices.Get<VertexFormat>();
}

//==============================================================================
template<class VertexFormat, class Base>
inline
const VertexFormat* Mesh<VertexFormat, Base>::GetVertices() const
{
  return m_vertices.Get<VertexFormat>();
}

//==============================================================================
template <class VertexFormat, class Base>
inline
void Mesh<VertexFormat, Base>::CreateVbo(uint32_t flags)
{
  Gfx::Destroy(m_vbo);
  XR_ASSERT(Mesh, m_vertices.GetRaw() != nullptr);
  m_vbo = Gfx::CreateVertexBuffer(Vertex::Formats::GetHandle<VertexFormat>(),
    GetVertexBuffer(), flags);
}

//==============================================================================
template <class VertexFormat, class Base>
typename Mesh<VertexFormat, Base>::PosType
  Mesh<VertexFormat, Base>::CalculateCentre() const
{
  PosType centre = PosType::Zero();
  const auto n = m_vertices.GetNumElements();
  if (n > 0)
  {
    for (auto i0 = GetVertices(), i1 = i0 + n; i0 != i1; ++i0)
    {
      centre += i0->pos;
    }
    centre *= 1.0f / n;
  }
  return centre;
}

//==============================================================================
template <class VertexFormat, class Base>
float Mesh<VertexFormat, Base>::CalculateRadius() const
{
  float radius = .0f;
  const auto n = m_vertices.GetNumElements();
  if (n > 0)
  {
    for (auto i0 = GetVertices(), i1 = i0 + n; i0 != i1; ++i0)
    {
      float d = i0->pos.Dot();
      if (d > radius)
      {
        radius = d;
      }
    }
    radius = sqrtf(radius);
  }
  return radius;
}

//==============================================================================
template <class VertexFormat, class Base>
void  Mesh<VertexFormat, Base>::CalculateExtents(PosType& minOut, PosType& maxOut) const
{
  const auto n = m_vertices.GetNumElements();
  if (n > 0)
  {
    for (int i = 0; i < PosType::kNumComponents; ++i)
    {
      minOut.arData[i] = std::numeric_limits<float>::max();
      maxOut.arData[i] = std::numeric_limits<float>::min();
    }

    for (auto i0 = GetVertices(), i1 = i0 + n; i0 != i1; ++i0)
    {
      for (int j = 0; j < PosType::kNumComponents; ++j)
      {
        if (i0->pos.arData[j] < minOut.arData[j])
        {
          minOut.arData[j] = i0->pos.arData[j];
        }

        if (i0->pos.arData[j] > maxOut.arData[j])
        {
          minOut.arData[j] = i0->pos.arData[j];
        }
      }
    }
  }
  else
  {
    minOut = maxOut = PosType::Zero();
  }
}

} // XR

#endif // XR_BASICMESH_HPP

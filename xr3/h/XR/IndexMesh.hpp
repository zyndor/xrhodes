#ifndef XR_INDEXMESH_HPP
#define XR_INDEXMESH_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "BasicMesh.hpp"
#include <vector>

namespace XR {

//==============================================================================
class IndexMeshCore
{
public:
  // types
  using IndexArray = std::vector<uint16_t>;

  // general
  IndexArray& GetIndices();
  const IndexArray& GetIndices() const;

  Buffer GetIndexBuffer() const;

  void  SetIndexPattern(const uint16_t* pInds, uint32_t numInds, uint32_t numSets);
  void  SetIndexPattern(const uint16_t* pInds, uint32_t numInds, uint16_t offset,
    uint32_t numSets);

  void CreateIbo(uint32_t flags = Gfx::F_BUFFER_NONE);

protected:
  // data
  IndexArray  m_indices;
  Gfx::IndexBufferHandle m_ibo;
};

//==============================================================================
template <class VertexFormat>
class IndexMesh:  public BasicMesh<VertexFormat>, IndexMeshCore
{
public:
  // types
  using Base = BasicMesh<VertexFormat>;

  // general
  using IndexMeshCore::GetIndices;
  using IndexMeshCore::GetIndexBuffer;
  using IndexMeshCore::SetIndexPattern;
  using IndexMeshCore::CreateIbo;

  ///@brief Renders the mesh using the index information, without setting the
  /// material.
  void RenderOnly(Primitive primitive) const;

  ///@brief Renders the mesh using @a count indices starting from @a offset,
  /// of the index information, without setting the material.
  void RenderOnly(Primitive primitive, uint32_t offset, uint32_t count) const;
};

//==============================================================================
// implementation
//==============================================================================
inline
IndexMeshCore::IndexArray&  IndexMeshCore::GetIndices()
{
  return m_indices;
}

//==============================================================================
inline
const IndexMeshCore::IndexArray&  IndexMeshCore::GetIndices() const
{
  return m_indices;
}

//==============================================================================
inline Buffer IndexMeshCore::GetIndexBuffer() const
{
  return Buffer {
    static_cast<uint32_t>(m_indices.size() * sizeof(IndexArray::value_type)),
    reinterpret_cast<uint8_t const*>(m_indices.data())
  };
}

//==============================================================================
template<class VertexFormat>
inline
void IndexMesh<VertexFormat>::RenderOnly(Primitive primitive) const
{
  RenderOnly(primitive, 0, static_cast<uint32_t>(m_indices.size()));
}

//==============================================================================
template<class VertexFormat>
inline
void IndexMesh<VertexFormat>::RenderOnly(Primitive primitive, uint32_t offset,
  uint32_t count) const
{
  XR_ASSERT(IndexMesh, Base::m_vbo.IsValid());
  XR_ASSERT(IndexMesh, m_ibo.IsValid());
  Gfx::Draw(Base::m_vbo, m_ibo, primitive, offset, count);
}

} // XR

#endif // XR_INDEXMESH_HPP

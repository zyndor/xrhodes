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

  void  SetIndexPattern(const uint16_t* pInds, int numInds, int repeat);
  void  SetIndexPattern(const uint16_t* pInds, int numInds, uint16_t shift,
    int repeat);

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
  using IndexMeshCore::SetIndexPattern;

  ///@brief Renders the mesh using the index information, without setting the
  /// material.
  void RenderOnly(PrimType primitiveType) const;

  ///@brief Renders the mesh using @a count indices starting from @a offset,
  /// of the index information, without setting the material.
  void RenderOnly(PrimType primitiveType, uint32_t offset, uint32_t count) const;
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
template<class VertexFormat>
inline
void IndexMesh<VertexFormat>::RenderOnly(PrimType primitiveType) const
{
  RenderOnly(primitiveType, 0, m_indices.size());
}

//==============================================================================
template<class VertexFormat>
inline
void IndexMesh<VertexFormat>::RenderOnly(PrimType primitiveType, uint32_t offset,
  uint32_t count) const
{
  XR_ASSERT(IndexMesh, Base::m_vbo.IsValid());
  XR_ASSERT(IndexMesh, m_ibo.IsValid());
  Gfx::Draw(Base::m_vbo, m_ibo, primitiveType, offset, count);
}

} // XR

#endif // XR_INDEXMESH_HPP

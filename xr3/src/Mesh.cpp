//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Mesh.hpp"

namespace XR {

//==============================================================================
BasicMesh::BasicMesh(uint32_t vertexSize, uint32_t numVertices)
: m_vertices(vertexSize, numVertices)
{}

//==============================================================================
BasicMesh::~BasicMesh()
{
  Gfx::Release(m_vbo);
}

//==============================================================================
IndexMesh::IndexMesh(uint32_t vertexSize, uint32_t numVertices)
: BasicMesh(vertexSize, numVertices)
{}

//==============================================================================
IndexMesh::~IndexMesh()
{
  Gfx::Release(m_ibo);
}

//==============================================================================
void  IndexMesh::SetIndexPattern(const uint16_t* indices, uint32_t indexCount,
  uint32_t numSets)
{
  XR_ASSERT(IndexMesh, indices != nullptr);

  m_indices.resize(indexCount * numSets);
  meshutil::SetIndexPattern(indices, indexCount, numSets, m_indices.data());
}

//==============================================================================
void IndexMesh::SetIndexPattern(const uint16_t* indices, uint32_t indexCount,
  uint16_t shift, uint32_t numSets)
{
  XR_ASSERT(IndexMesh, indices != nullptr);

  m_indices.resize(indexCount * numSets);
  meshutil::SetIndexPattern(indices, indexCount, shift, numSets, m_indices.data());
}

//==============================================================================
void IndexMesh::CreateIbo(uint32_t flags)
{
  Gfx::Release(m_ibo);
  m_ibo = Gfx::CreateIndexBuffer(GetIndexBuffer(), flags);
}


} // XR

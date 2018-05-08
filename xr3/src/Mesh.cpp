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
  Gfx::Destroy(m_vbo);
}

//==============================================================================
IndexMesh::IndexMesh(uint32_t vertexSize, uint32_t numVertices)
: BasicMesh(vertexSize, numVertices)
{}

//==============================================================================
IndexMesh::~IndexMesh()
{
  Gfx::Destroy(m_ibo);
}

//==============================================================================
void  IndexMesh::SetIndexPattern(const uint16_t* pInds, uint32_t numInds, uint32_t numSets)
{
  XR_ASSERT(IndexMesh, pInds != nullptr);

  uint16_t  shift = 0;
  for (decltype(numInds) i = 0; i < numInds; ++i)
  {
    if (pInds[i] > shift)
    {
      shift = pInds[i];
    }
  }

  SetIndexPattern(pInds, numInds, shift + 1, numSets);
}

//==============================================================================
void IndexMesh::SetIndexPattern(const uint16_t * pInds, uint32_t numInds,
  uint16_t shift, uint32_t numSets)
{
  XR_ASSERT(IndexMesh, pInds != nullptr);

  uint32_t indexOffset = 0;
  uint32_t vertexOffset = 0;
  m_indices.resize(numInds * numSets);
  for (decltype(numSets) i = 0; i < numSets; ++i)
  {
    for (decltype(numInds) j = 0; j < numInds; ++j)
    {
      m_indices[indexOffset + j] = pInds[j] + vertexOffset;
    }
    indexOffset += numInds;
    vertexOffset += shift;
  }
}

//==============================================================================
void IndexMesh::CreateIbo(uint32_t flags)
{
  Gfx::Destroy(m_ibo);
  m_ibo = Gfx::CreateIndexBuffer(GetIndexBuffer(), flags);
}


} // XR

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "IndexMesh.hpp"

namespace XR {

//==============================================================================
IndexMesh::IndexMesh()
: BasicMesh(),
  m_indices()
{}

//==============================================================================
IndexMesh::~IndexMesh()
{}

//==============================================================================
void  IndexMesh::SetIndexPattern(const uint16_t* pInds, int numInds, int repeat)
{
  XR_ASSERT(IndexMesh, numInds >= 0);
  XR_ASSERT(IndexMesh, repeat >= 0);
  XR_ASSERT(IndexMesh, pInds != 0);
  
  uint16_t  shift(0);
  for (int i = 0; i < numInds; ++i)
  {
    if (pInds[i] > shift)
    {
      shift = pInds[i];
    }
  }
  
  SetIndexPattern(pInds, numInds, shift + 1, repeat);
}

//==============================================================================
void  IndexMesh::SetIndexPattern(const uint16_t* pInds, int numInds, uint16_t shift,
    int repeat)
{
  XR_ASSERT(IndexMesh, numInds >= 0);
  XR_ASSERT(IndexMesh, repeat >= 0);
  XR_ASSERT(IndexMesh, pInds != 0);
  
  int indexOffset(0);
  int vertexOffset(0);
  m_indices.resize(numInds * repeat);
  for (int i = 0; i < repeat; ++i)
  {
    for (int j = 0; j < numInds; ++j)
    {
      m_indices[indexOffset + j] = pInds[j] + vertexOffset;
    }
    indexOffset += numInds;
    vertexOffset += shift;
  }
}

//==============================================================================
void  IndexMesh::Render()
{
  if (m_vertices.GetCapacity() > 0)
  {
    _PrepareRender();
    Renderer::DrawPrims(PRIM_TRI_LIST, &m_indices[0], m_indices.size());
  }
}

} // XR

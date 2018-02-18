//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/IndexMesh.hpp"
#include "XR/Renderer.hpp"

namespace XR {

//==============================================================================
void  IndexMeshCore::SetIndexPattern(const uint16_t* pInds, int numInds, int numSets)
{
  XR_ASSERT(IndexMesh, numInds >= 0);
  XR_ASSERT(IndexMesh, numSets >= 0);
  XR_ASSERT(IndexMesh, pInds != 0);

  uint16_t  shift(0);
  for (int i = 0; i < numInds; ++i)
  {
    if (pInds[i] > shift)
    {
      shift = pInds[i];
    }
  }

  SetIndexPattern(pInds, numInds, shift + 1, numSets);
}

//==============================================================================
void IndexMeshCore::SetIndexPattern(const uint16_t * pInds, int numInds,
  uint16_t shift, int numSets)
{
  XR_ASSERT(IndexMesh, numInds >= 0);
  XR_ASSERT(IndexMesh, numSets >= 0);
  XR_ASSERT(IndexMesh, pInds != 0);

  int indexOffset(0);
  int vertexOffset(0);
  m_indices.resize(numInds * numSets);
  for (int i = 0; i < numSets; ++i)
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
void IndexMeshCore::CreateIbo(uint32_t flags)
{
  Gfx::Destroy(m_ibo);
  m_ibo = Gfx::CreateIndexBuffer(GetIndexBuffer(), flags);
}

} // XR

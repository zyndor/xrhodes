//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/IndexMesh.hpp"
#include <algorithm>

namespace xr
{

//==============================================================================
IndexMesh::IndexMesh()
: Mesh(),
  numIndices(0)
{}

//==============================================================================
IndexMesh::IndexMesh(uint32_t numVertices, Gfx::VertexBufferHandle hVb,
  uint32_t numIndices, Gfx::IndexBufferHandle hIb)
: Mesh(numVertices, hVb),
  numIndices(numIndices),
  hIndexBuffer(hIb)
{}

//==============================================================================
IndexMesh::IndexMesh(uint32_t numVertices, Gfx::VertexBufferHandle hVb)
: IndexMesh(numVertices, hVb, 0, Gfx::IndexBufferHandle())
{}

//==============================================================================
IndexMesh::IndexMesh(IndexMesh && other)
: Mesh(std::move(other)),
  numIndices(other.numIndices),
  hIndexBuffer(other.hIndexBuffer)
{
  other.numIndices = 0;
  other.hIndexBuffer.Invalidate();
}

//==============================================================================
IndexMesh::~IndexMesh()
{
  Gfx::Release(hIndexBuffer);
}

//==============================================================================
void IndexMesh::Render(Primitive prim) const
{
  Render(prim, 0, numIndices);
}

//==============================================================================
void IndexMesh::Render(Primitive prim, uint32_t offset, uint32_t count) const
{
  Gfx::Draw(hVertexBuffer, hIndexBuffer, prim, offset, count);
}

//==============================================================================
void IndexMesh::Swap(IndexMesh& other)
{
  Mesh::Swap(other);
  std::swap(numIndices, other.numIndices);
  std::swap(hIndexBuffer, other.hIndexBuffer);
}

//==============================================================================
IndexMesh& IndexMesh::operator=(IndexMesh&& other)
{
  IndexMesh tmp(std::move(other));
  Swap(tmp);
  return *this;
}

} // xr

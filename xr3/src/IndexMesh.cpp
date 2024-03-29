//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
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
IndexMesh::IndexMesh(uint32_t numVertices_, Gfx::VertexBufferHandle hVb,
  uint32_t numIndices_, Gfx::IndexBufferHandle hIb)
: Mesh(numVertices_, hVb),
  numIndices(numIndices_),
  hIndexBuffer(hIb)
{}

//==============================================================================
IndexMesh::IndexMesh(uint32_t numVertices_, Gfx::VertexBufferHandle hVb)
: IndexMesh(numVertices_, hVb, 0, Gfx::IndexBufferHandle())
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

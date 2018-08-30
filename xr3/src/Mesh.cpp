//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Mesh.hpp"
#include <algorithm>

namespace xr
{

//==============================================================================
Mesh::Mesh()
: numVertices(0)
{}

//==============================================================================
Mesh::Mesh(uint32_t numVertices, Gfx::VertexBufferHandle hVb)
: numVertices(numVertices),
  hVertexBuffer(hVb)
{}

//==============================================================================
Mesh::Mesh(Mesh && other)
: numVertices(other.numVertices),
  hVertexBuffer(other.hVertexBuffer)
{
  other.numVertices = 0;
  other.hVertexBuffer.Invalidate();
}

//==============================================================================
Mesh::~Mesh()
{
  Gfx::Release(hVertexBuffer);
}

//==============================================================================
void Mesh::Render(Primitive prim) const
{
  Render(prim, 0, numVertices);
}

//==============================================================================
void Mesh::Render(Primitive prim, uint32_t offset, uint32_t count) const
{
  Gfx::Draw(hVertexBuffer, prim, offset, count);
}

//==============================================================================
void Mesh::Swap(Mesh& other)
{
  std::swap(numVertices, other.numVertices);
  std::swap(hVertexBuffer, other.hVertexBuffer);
}

//==============================================================================
Mesh& Mesh::operator=(Mesh&& other)
{
  Mesh tmp(std::move(other));
  Swap(tmp);
  return *this;
}

} // xr

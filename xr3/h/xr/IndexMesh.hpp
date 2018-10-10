#ifndef XR_INDEXMESH_HPP
#define XR_INDEXMESH_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Mesh.hpp"
#include "xr/Gfx.hpp"
#include "xr/Buffer.hpp"

namespace xr
{

//==============================================================================
///@brief Mesh with an index buffer handle and number of indices.
class IndexMesh : public Mesh
{
public:
  // static
  ///@return IndexMesh created with @a numVertices vertices of the given
  /// @a vertexData, @a numIndices of 16-bit @a indexData, and a combination of
  /// Gfx F_BUFFER_* @a flags for the vertex buffer.
  template <class VertexT>
  static IndexMesh Create(uint32_t numVertices, VertexT const* vertexData,
    uint32_t numIndices, uint16_t const* indexData,
    Gfx::FlagType vertexFlags = Gfx::F_BUFFER_NONE);

  ///@return IndexMesh created with @a numVertices vertices of the given
  /// @a vertexData, @a numIndices of 32-bit @a indexData, and a combination of
  /// Gfx F_BUFFER_* @a flags for the vertex buffer.
  template <class VertexT>
  static IndexMesh Create(uint32_t numVertices, VertexT const* vertexData,
    uint32_t numIndices, uint32_t const* indexData,
    Gfx::FlagType vertexFlags = Gfx::F_BUFFER_NONE);

  // data
  uint32_t numIndices;
  Gfx::IndexBufferHandle hIndexBuffer;

  // structors
  IndexMesh();
  IndexMesh(uint32_t numVertices, Gfx::VertexBufferHandle hVb, uint32_t numIndices,
    Gfx::IndexBufferHandle);
  IndexMesh(uint32_t numVertices, Gfx::VertexBufferHandle hVb);
  IndexMesh(IndexMesh&& other);
  ~IndexMesh();

  // general
  ///@brief Renders all of the indices as primitive type @a prim.
  ///@note A Shader and states must be set prior to this.
  void  Render(Primitive prim) const override;

  ///@brief Renders the vertices indexed by @a count of the indices, starting
  /// from @a offset, as primitive type @a prim.
  ///@note A Shader and states must be set prior to this.
  void  Render(Primitive prim, uint32_t offset, uint32_t count) const override;

  ///@brief Swaps this and @a other efficiently.
  void Swap(IndexMesh& other);

  void Swap(Mesh& other) = delete;  // avoid slicing by mistake.

  // operators
  IndexMesh& operator=(IndexMesh&& other);
};

//==============================================================================
// inline
//==============================================================================
template<class VertexT>
inline
IndexMesh IndexMesh::Create(uint32_t numVertices, VertexT const* vertexData,
  uint32_t numIndices, uint16_t const * indexData, Gfx::FlagType vertexFlags)
{
  Gfx::VertexBufferHandle hVbo = MakeVertexBuffer(numVertices, vertexData, vertexFlags);
  Buffer indexBuffer = { numIndices * sizeof(uint16_t),
    reinterpret_cast<uint8_t const*>(indexData) };
  Gfx::IndexBufferHandle hIbo = Gfx::CreateIndexBuffer(indexBuffer);
  return IndexMesh(numVertices, hVbo, numIndices, hIbo);
}

//==============================================================================
template<class VertexT>
inline
IndexMesh IndexMesh::Create(uint32_t numVertices, VertexT const* vertexData,
  uint32_t numIndices, uint32_t const * indexData, Gfx::FlagType vertexFlags)
{
  Gfx::VertexBufferHandle hVbo = MakeVertexBuffer(numVertices, vertexData, vertexFlags);
  Buffer indexBuffer = { numIndices * sizeof(uint16_t),
    reinterpret_cast<uint8_t const*>(indexData) };
  Gfx::IndexBufferHandle hIbo = Gfx::CreateIndexBuffer(indexBuffer,
    Gfx::F_BUFFER_INDEX_32BITS);
  return IndexMesh(numVertices, hVbo, numIndices, hIbo);
}

} // xr

#endif //XR_INDEXMESH_HPP

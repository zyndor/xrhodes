#ifndef XR_MESH_HPP
#define XR_MESH_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/VertexFormats.hpp"
#include "xr/Gfx.hpp"
#include "xr/Buffer.hpp"

namespace xr
{

//==============================================================================
///@brief Smart handle to a vertex buffer and the number of vertices it was
/// created with. Does not retain other data.
class Mesh
{
public:
  // static
  ///@brief Creates a vertex buffer from an array of @a numVertices vertices in
  /// @a vertexData, and using @a flags.
  ///@return Handle to and ownership of, the resulting vertex buffer.
  template <class VertexT>
  static Gfx::VertexBufferHandle MakeVertexBuffer(uint32_t numVertices,
    VertexT const* vertexData, Gfx::FlagType flags = Gfx::F_BUFFER_NONE);

  ///@return Mesh created with @a numVertices vertices of the given @a vertexData
  /// and a combination of Gfx F_BUFFER_* @a flags.
  template <class VertexT>
  static Mesh Create(uint32_t numVertices, VertexT const* vertexData,
    Gfx::FlagType flags = Gfx::F_BUFFER_NONE);

  // data
  uint32_t numVertices;
  Gfx::VertexBufferHandle hVertexBuffer;

  // structors
  Mesh();
  Mesh(uint32_t numVertices, Gfx::VertexBufferHandle hVb);
  Mesh(Mesh&& other);
  virtual ~Mesh();

  // general
  ///@brief Renders all of the vertices as primitive type @a prim.
  ///@note A Shader and states must be set prior to this.
  virtual void  Render(Primitive prim) const;

  ///@brief Renders @a count vertices from offset @a offset into the vertex
  /// buffer that this has, as primitive type @a prim.
  ///@note A Shader and states must be set prior to this.
  virtual void  Render(Primitive prim, uint32_t offset, uint32_t count) const;

  ///@brief Swaps this and @a other efficiently.
  void Swap(Mesh& other);

  // operators
  Mesh& operator=(Mesh&& other);
};

//==============================================================================
// inline
//==============================================================================
template<class VertexT>
inline
Gfx::VertexBufferHandle Mesh::MakeVertexBuffer(uint32_t numVertices,
  VertexT const* vertexData, Gfx::FlagType flags)
{
  Buffer vertexBuffer = { numVertices * VertexT::kSize,
    reinterpret_cast<uint8_t const*>(vertexData) };
  auto hFormat = Vertex::Formats::GetHandle<VertexT>();
  auto hVertexBuffer = Gfx::CreateVertexBuffer(hFormat, vertexBuffer, flags);
  return hVertexBuffer;
}

//==============================================================================
template<class VertexT>
inline
Mesh  Mesh::Create(uint32_t numVertices, VertexT const* vertexData, Gfx::FlagType flags)
{
  return Mesh(numVertices, MakeVertexBuffer(numVertices, vertexData, flags));
}

} // xr

#endif //XR_MESH_HPP

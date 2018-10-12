#ifndef XR_SCRATCHBUFFER_HPP
#define XR_SCRATCHBUFFER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Vertex.hpp"
#include "xr/types/fundamentals.hpp"

namespace xr
{

///@brief ScratchBuffer provides immediate rendering with transient vertex and
/// index buffer objects that it manages.
class ScratchBuffer
{
  XR_NONCOPY_DECL(ScratchBuffer)

public:
  static void Init(size_t poolSize);

  static void* Allocate(size_t numBytes);

  using Position2d = Vertex::Format<Vertex::Pos<Vector2>>;
  static Position2d* Start2d(uint32_t numVertices);

  using Position3d = Vertex::Format<Vertex::Pos<Vector3>>;
  static Position3d* Start3d(uint32_t numVertices);

  using Position2dUv = Vertex::Format<Position2d, Vertex::UV0<Vector2>>;
  static Position2dUv* Start2dUv(uint32_t numVertices);

  using Position3dUv = Vertex::Format<Position3d, Vertex::UV0<Vector2>>;
  static Position3dUv* Start3dUv(uint32_t numVertices);

  using Position2dUvColor = Vertex::Format<Vertex::Color0<Color>, Position2dUv>;
  static Position2dUvColor* Start2dUvColor(uint32_t numVertices);

  using Position3dUvColor = Vertex::Format<Vertex::Color0<Color>, Position3dUv>;
  static Position3dUvColor* Start3dUvColor(uint32_t numVertices);

  using Position2dColor = Vertex::Format<Vertex::Color0<Color>, Position2d>;
  static Position2dColor* Start2dColor(uint32_t numVertices);

  using Position3dColor = Vertex::Format<Vertex::Color0<Color>, Position3d>;
  static Position3dColor* Start3dColor(uint32_t numVertices);

  static uint16_t* AllocateIndices(uint32_t numIndices);

  static void Finish(Primitive primitive);

  static void Flush();
};

}

#endif //XR_SCRATCHBUFFER_HPP

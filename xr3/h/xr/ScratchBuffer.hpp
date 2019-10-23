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

//==============================================================================
///@brief ScratchBuffer provides immediate rendering with transient vertex and
/// index buffer objects that it manages.
class ScratchBuffer
{
  XR_NONCOPY_DECL(ScratchBuffer)

public:
  struct Handles
  {
    Gfx::VertexBufferHandle mVbo;
    Gfx::IndexBufferHandle mIbo;
  };

  ///@brief Initializes the ScratchBuffer with @a poolSize bytes of memory and
  /// registers for Gfx's shutdown signal for clean up.
  static void Init(size_t poolSize);

  [[deprecated("Returns nullptr")]]
  static void* Allocate(size_t numBytes);

  using Position2d= Vertex::Format<Vertex::Pos<Vector2>>;
  ///@brief Starts a scratch with a vertex format composed of 2d position only.
  static Position2d* Start2d(uint32_t numVertices);

  using Position3d= Vertex::Format<Vertex::Pos<Vector3>>;
  ///@brief Starts a scratch with a vertex format composed of 3d position only.
  static Position3d* Start3d(uint32_t numVertices);

  using Position2dUv= Vertex::Format<Position2d, Vertex::UV0<Vector2>>;
  ///@brief Starts a scratch with a vertex format composed of 2d position and UVs.
  static Position2dUv* Start2dUv(uint32_t numVertices);

  using Position3dUv= Vertex::Format<Position3d, Vertex::UV0<Vector2>>;
  ///@brief Starts a scratch with a vertex format composed of 3d position and UVs.
  static Position3dUv* Start3dUv(uint32_t numVertices);

  using Position2dUvColor= Vertex::Format<Vertex::Color0<Color>, Position2dUv>;
  ///@brief Starts a scratch with a vertex format composed of 2d position, UVs
  /// and color.
  static Position2dUvColor* Start2dUvColor(uint32_t numVertices);

  using Position3dUvColor= Vertex::Format<Vertex::Color0<Color>, Position3dUv>;
  ///@brief Starts a scratch with a vertex format composed of 3d position, UVs
  /// and color.
  static Position3dUvColor* Start3dUvColor(uint32_t numVertices);

  using Position2dColor= Vertex::Format<Vertex::Color0<Color>, Position2d>;
  ///@brief Starts a scratch with a vertex format composed of 2d position and color.
  static Position2dColor* Start2dColor(uint32_t numVertices);

  using Position3dColor= Vertex::Format<Vertex::Color0<Color>, Position3d>;
  ///@brief Starts a scratch with a vertex format composed of 3d position and color.
  static Position3dColor* Start3dColor(uint32_t numVertices);

  ///@brief Allocates memory for @a count 16-bit indices from the pool.
  ///@note A subsequent BakeIndices() or Finish() call invalidates this memory.
  static uint16_t* AllocateIndices(uint32_t count);

  ///@brief Creates an index buffer object from the scratch data, which is
  /// then consumed.
  ///@note Requires AllocateIndices() methods to be called beforehand
  /// (and no BakeIndices() or Finish() since that).
  ///@note The number of indices is whatever was supplied to the preceding
  /// AllocateIndices().
  static Handles Bake();

  ///@brief Renders and consumes the scratch buffer data.
  static void Finish(Primitive primitive);

  [[deprecated("No-op.")]]
  static void Flush();
};

}

#endif //XR_SCRATCHBUFFER_HPP

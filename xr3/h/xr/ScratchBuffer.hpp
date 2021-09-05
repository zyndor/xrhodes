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
    Gfx::VertexBufferHandle mVbo{};
    Gfx::IndexBufferHandle mIbo{};
  };

  ///@brief Initializes the ScratchBuffer with @a poolSize bytes of memory and
  /// registers for Gfx's shutdown signal for clean up.
  static void Init(size_t poolSize);

  [[deprecated("Returns nullptr")]]
  static void* Allocate(size_t numBytes);

  using Position2d= Vertex::Format<Vertex::Pos<Vector2>>;
  ///@brief Starts a scratch with a vertex format composed of 2d position only.
  [[nodiscard]] static Position2d* Start2d(uint32_t numVertices);

  using Position3d= Vertex::Format<Vertex::Pos<Vector3>>;
  ///@brief Starts a scratch with a vertex format composed of 3d position only.
  [[nodiscard]] static Position3d* Start3d(uint32_t numVertices);

  using Position2dUv= Vertex::Format<Position2d, Vertex::UV0<Vector2>>;
  ///@brief Starts a scratch with a vertex format composed of 2d position and UVs.
  [[nodiscard]] static Position2dUv* Start2dUv(uint32_t numVertices);

  using Position3dUv= Vertex::Format<Position3d, Vertex::UV0<Vector2>>;
  ///@brief Starts a scratch with a vertex format composed of 3d position and UVs.
  [[nodiscard]] static Position3dUv* Start3dUv(uint32_t numVertices);

  using Position2dUvColor= Vertex::Format<Vertex::Color0<Color>, Position2dUv>;
  ///@brief Starts a scratch with a vertex format composed of 2d position, UVs
  /// and color.
  [[nodiscard]] static Position2dUvColor* Start2dUvColor(uint32_t numVertices);

  using Position3dUvColor= Vertex::Format<Vertex::Color0<Color>, Position3dUv>;
  ///@brief Starts a scratch with a vertex format composed of 3d position, UVs
  /// and color.
  [[nodiscard]] static Position3dUvColor* Start3dUvColor(uint32_t numVertices);

  using Position2dColor= Vertex::Format<Vertex::Color0<Color>, Position2d>;
  ///@brief Starts a scratch with a vertex format composed of 2d position and color.
  [[nodiscard]] static Position2dColor* Start2dColor(uint32_t numVertices);

  using Position3dColor= Vertex::Format<Vertex::Color0<Color>, Position3d>;
  ///@brief Starts a scratch with a vertex format composed of 3d position and color.
  [[nodiscard]] static Position3dColor* Start3dColor(uint32_t numVertices);

  ///@brief Allocates memory for @a count 16-bit indices from the pool.
  ///@note Does not transfer ownership.
  ///@note A subsequent Bake() or Finish() call invalidates this memory.
  ///@note Overrides any previously specified index buffer.
  [[nodiscard]] static uint16_t* AllocateIndices(uint32_t count);

  ///@brief Provides pre-calculated indices to use for the next draw call.
  ///@note Does not transfer ownership.
  ///@note A subsequent Bake() or Finish() call consumes the pointer set.
  ///@note Overrides any previously specified index buffer.
  static void UseIndices(uint32_t count, uint16_t const* data);

  ///@brief Uploads the vertex, and if AllocateIndices() or UseIndices() was called,
  /// the index buffer to the GPU, returning the resulting pair of handles, along
  /// with ownership.
  ///@note One of the Start methods must have been called beforehand.
  ///@note Invalidates the buffers, i.e. client code must not call Finish().
  ///@note The vertex / index count is whatever was supplied to the Start and
  /// Allocate- or UseIndices() methods prior to Bake().
  [[nodiscard]] static Handles Bake();

  ///@brief Renders and consumes the scratch buffer data.
  static void Finish(Primitive primitive);

  [[deprecated("No-op.")]]
  static void Flush();
};

}

#endif //XR_SCRATCHBUFFER_HPP

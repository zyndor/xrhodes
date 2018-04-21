//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/ScratchBuffer.hpp"
#include "XR/VertexFormats.hpp"
#include "XR/Pool.hpp"

namespace XR
{
namespace
{

struct ScratchBufferImpl
{
  ScratchBufferImpl(size_t poolSize)
  {
    m_pool.SetBuffer(poolSize, true, nullptr);
  }

  void* Allocate(size_t numBytes)
  {
    return m_pool.Allocate(numBytes);
  }

  template <class VertexFormat>
  VertexFormat* AllocateVertices(uint32_t numVertices)
  {
    XR_ASSERTMSG(ScratchBuffer, !m_hFormat.IsValid(), ("Call Finish() before starting a new scratch!"));
    size_t size = numVertices * VertexFormat::kSize;
    auto buffer = m_pool.Allocate(size);
    if (buffer)
    {
      m_hFormat = Vertex::Formats::GetHandle<VertexFormat>();
      m_numVertices = numVertices;
      m_vertices = { size, static_cast<uint8_t*>(buffer) };
    }
    return static_cast<VertexFormat*>(buffer);
  }

  uint16_t* AllocateIndices(uint32_t numIndices)
  {
    XR_ASSERTMSG(ScratchBuffer, !m_indices.data, ("Call Finish() before starting a new scratch!"));
    size_t size = numIndices * sizeof(uint16_t);
    auto buffer = m_pool.Allocate(size);
    if (buffer)
    {
      m_numIndices = numIndices;
      m_indices = { size, static_cast<uint8_t*>(buffer) };
    }
    return static_cast<uint16_t*>(buffer);
  }

  void RenderBuffers(Primitive primitive)
  {
    XR_ASSERTMSG(ScratchBuffer, m_vertices.data, ("No vertices allocated for drawing."));
    auto vbo = Gfx::CreateVertexBuffer(m_hFormat, m_vertices);
    if (m_indices.data)
    {
      auto ibo = Gfx::CreateIndexBuffer(m_indices);
      Gfx::Draw(vbo, ibo, primitive, 0, m_numIndices);

      Gfx::Destroy(ibo);
    }
    else
    {
      Gfx::Draw(vbo, primitive, 0, m_numVertices);
    }

    Gfx::Destroy(vbo);

    m_vertices = { 0, nullptr };
    m_indices = { 0, nullptr };
    m_hFormat.Invalidate();
  }

  void Flush()
  {
    m_pool.Flush();
  }

private:
  Pool m_pool;

  Gfx::VertexFormatHandle m_hFormat;  // no ownership
  uint32_t m_numVertices = 0;
  Buffer m_vertices = { 0, nullptr };

  uint32_t m_numIndices = 0;
  Buffer m_indices = { 0, nullptr };
};

ScratchBufferImpl*  s_impl = nullptr;

} //

//==============================================================================
void ScratchBuffer::Init(size_t poolSize)
{
  s_impl = new ScratchBufferImpl(poolSize);

  Gfx::RegisterFlushCallback([](void*, void*) {
    Flush();
  }, nullptr);

  Gfx::RegisterExitCallback([](void*, void*) {
    delete s_impl;
    s_impl = nullptr;
  }, nullptr);
}

//==============================================================================
void* ScratchBuffer::Allocate(size_t numBytes)
{
  return s_impl->Allocate(numBytes);
}

//==============================================================================
ScratchBuffer::Position2d* ScratchBuffer::Start2d(uint32_t numVertices)
{
  return s_impl->AllocateVertices<Position2d>(numVertices);
}

//==============================================================================
ScratchBuffer::Position3d * ScratchBuffer::Start3d(uint32_t numVertices)
{
  return s_impl->AllocateVertices<Position3d>(numVertices);
}

//==============================================================================
ScratchBuffer::Position2dUv * ScratchBuffer::Start2dUv(uint32_t numVertices)
{
  return s_impl->AllocateVertices<Position2dUv>(numVertices);
}

//==============================================================================
ScratchBuffer::Position3dUv * ScratchBuffer::Start3dUv(uint32_t numVertices)
{
  return s_impl->AllocateVertices<Position3dUv>(numVertices);
}

//==============================================================================
ScratchBuffer::Position2dUvColor * ScratchBuffer::Start2dUvColor(uint32_t numVertices)
{
  return s_impl->AllocateVertices<Position2dUvColor>(numVertices);
}

//==============================================================================
ScratchBuffer::Position3dUvColor * ScratchBuffer::Start3dUvColor(uint32_t numVertices)
{
  return s_impl->AllocateVertices<Position3dUvColor>(numVertices);
}

//==============================================================================
ScratchBuffer::Position2dColor * ScratchBuffer::Start2dColor(uint32_t numVertices)
{
  return s_impl->AllocateVertices<Position2dColor>(numVertices);
}

//==============================================================================
ScratchBuffer::Position3dColor * ScratchBuffer::Start3dColor(uint32_t numVertices)
{
  return s_impl->AllocateVertices<Position3dColor>(numVertices);
}

//==============================================================================
uint16_t * ScratchBuffer::AllocateIndices(uint32_t numIndices)
{
  return s_impl->AllocateIndices(numIndices);
}

//==============================================================================
void ScratchBuffer::Finish(Primitive primitive)
{
  s_impl->RenderBuffers(primitive);
}

//==============================================================================
void ScratchBuffer::Flush()
{
  s_impl->Flush();
}

} // XR

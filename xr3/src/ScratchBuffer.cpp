//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/ScratchBuffer.hpp"
#include "xr/VertexFormats.hpp"
#include "xr/memory/Pool.hpp"

namespace xr
{
namespace
{

class ScratchBufferImpl
{
public:
  ScratchBufferImpl(size_t poolSize)
  {
    mPool.SetBuffer(poolSize, true, nullptr);
  }

  template <class VertexFormat>
  VertexFormat* AllocateVertices(uint32_t numVertices)
  {
    XR_ASSERTMSG(ScratchBuffer, !mFormat.IsValid(), ("Call Finish() before starting a new scratch!"));
    size_t size = numVertices * VertexFormat::kSize;
    auto buffer = mPool.Allocate(size);
    if (buffer)
    {
      mFormat = Vertex::Formats::GetHandle<VertexFormat>();
      mNumVertices = numVertices;
      mVertices = { size, static_cast<uint8_t*>(buffer) };
    }
    return static_cast<VertexFormat*>(buffer);
  }

  uint16_t* AllocateIndices(uint32_t numIndices)
  {
    XR_ASSERTMSG(ScratchBuffer, mVertices.data, ("Call Start{...}() before allocating indices!"));
    XR_ASSERTMSG(ScratchBuffer, !mIndices.data, ("Call Finish() before starting a new scratch!"));
    size_t size = numIndices * sizeof(uint16_t);
    auto buffer = mPool.Allocate(size);
    if (buffer)
    {
      mNumIndices = numIndices;
      mIndices = { size, static_cast<uint8_t*>(buffer) };
    }
    return static_cast<uint16_t*>(buffer);
  }

  void UseIndices(uint32_t count, uint16_t const* data)
  {
    mNumIndices = count;
    mIndices = Buffer::FromArray(count, data);
  }

  ScratchBuffer::Handles Bake()
  {
    XR_ASSERTMSG(ScratchBuffer, mVertices.data, ("No vertices allocated for drawing."));
    ScratchBuffer::Handles result{ Gfx::CreateVertexBuffer(mFormat, mVertices) };
    mVertices = { 0, nullptr };
    mFormat.Invalidate();
    if (mIndices.data)
    {
      result.mIbo = Gfx::CreateIndexBuffer(mIndices);
      mIndices = { 0, nullptr };
    }
    mPool.Flush();
    return result;
  }

  void RenderBuffers(Primitive primitive)
  {
    XR_ASSERTMSG(ScratchBuffer, mVertices.data, ("No vertices allocated for drawing."));
    auto handles = Bake();
    if (handles.mIbo.IsValid())
    {
      Gfx::Draw(handles.mVbo, handles.mIbo, primitive, 0, mNumIndices);
      Gfx::Release(handles.mIbo);
    }
    else
    {
      Gfx::Draw(handles.mVbo, primitive, 0, mNumVertices);
    }

    Gfx::Release(handles.mVbo);
  }

private:
  Pool mPool;

  Gfx::VertexFormatHandle mFormat;  // no ownership
  uint32_t mNumVertices = 0;
  Buffer mVertices{ 0, nullptr };

  uint32_t mNumIndices = 0;
  Buffer mIndices{ 0, nullptr };
};

ScratchBufferImpl*  sImpl = nullptr;

} // nonamespace

//==============================================================================
void ScratchBuffer::Init(size_t poolSize)
{
  XR_ASSERTMSG(ScratchBuffer, !sImpl, ("Already initialized!"));
  sImpl = new ScratchBufferImpl(poolSize);

  Gfx::ShutdownSignal().Connect(FunctionPtrCallback<void>([](void*) {
    delete sImpl;
    sImpl = nullptr;
  }, nullptr));
}

//==============================================================================
void* ScratchBuffer::Allocate(size_t /*numBytes*/)
{
  return nullptr;
}

//==============================================================================
ScratchBuffer::Position2d* ScratchBuffer::Start2d(uint32_t numVertices)
{
  return sImpl->AllocateVertices<Position2d>(numVertices);
}

//==============================================================================
ScratchBuffer::Position3d* ScratchBuffer::Start3d(uint32_t numVertices)
{
  return sImpl->AllocateVertices<Position3d>(numVertices);
}

//==============================================================================
ScratchBuffer::Position2dUv* ScratchBuffer::Start2dUv(uint32_t numVertices)
{
  return sImpl->AllocateVertices<Position2dUv>(numVertices);
}

//==============================================================================
ScratchBuffer::Position3dUv* ScratchBuffer::Start3dUv(uint32_t numVertices)
{
  return sImpl->AllocateVertices<Position3dUv>(numVertices);
}

//==============================================================================
ScratchBuffer::Position2dUvColor* ScratchBuffer::Start2dUvColor(uint32_t numVertices)
{
  return sImpl->AllocateVertices<Position2dUvColor>(numVertices);
}

//==============================================================================
ScratchBuffer::Position3dUvColor* ScratchBuffer::Start3dUvColor(uint32_t numVertices)
{
  return sImpl->AllocateVertices<Position3dUvColor>(numVertices);
}

//==============================================================================
ScratchBuffer::Position2dColor* ScratchBuffer::Start2dColor(uint32_t numVertices)
{
  return sImpl->AllocateVertices<Position2dColor>(numVertices);
}

//==============================================================================
ScratchBuffer::Position3dColor* ScratchBuffer::Start3dColor(uint32_t numVertices)
{
  return sImpl->AllocateVertices<Position3dColor>(numVertices);
}

//==============================================================================
uint16_t* ScratchBuffer::AllocateIndices(uint32_t numIndices)
{
  return sImpl->AllocateIndices(numIndices);
}

//==============================================================================
void ScratchBuffer::UseIndices(uint32_t count, uint16_t const* data)
{
  sImpl->UseIndices(count, data);
}

//==============================================================================
ScratchBuffer::Handles ScratchBuffer::Bake()
{
  return sImpl->Bake();
}

//==============================================================================
void ScratchBuffer::Finish(Primitive primitive)
{
  sImpl->RenderBuffers(primitive);
}

//==============================================================================
void ScratchBuffer::Flush()
{
}

} // xr

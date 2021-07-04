//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "GfxS.hpp"
#include "GfxCore.hpp"
#include "xr/debug.hpp"
#include "xr/events/SignalBroadcaster.hpp"

namespace xr
{
namespace Gfx
{
namespace
{

ResourceManager* sResources = nullptr;

} // nonamespace

//=============================================================================
void S::Init(Context* context, ResourceManager* resources)
{
  sResources = resources;
  Core::Init(context, resources);
}

//=============================================================================
VertexFormatHandle S::RegisterVertexFormat(VertexFormat const& format)
{
  return sResources->RegisterVertexFormat(format);
}

//=============================================================================
void S::Release(VertexFormatHandle h)
{
  XR_ASSERT(Gfx, h.IsValid());
  Core::Release(h);
}

//=============================================================================
VertexBufferHandle S::CreateVertexBuffer(VertexFormatHandle hFormat, Buffer const& buffer,
  FlagType flags)
{
  auto& vbos = sResources->GetVbos();
  VertexBufferHandle h{ static_cast<uint16_t>(vbos.server.Acquire()) };
  Core::CreateVertexBuffer(hFormat, buffer, flags, vbos[h.id]);
  return h;
}

//=============================================================================
void S::Release(VertexBufferHandle h)
{
  XR_ASSERT(Gfx, h.IsValid());
  Core::Release(h);
}

//=============================================================================
IndexBufferHandle S::CreateIndexBuffer(Buffer const& buffer, FlagType flags)
{
  auto& ibos = sResources->GetIbos();
  IndexBufferHandle h{ static_cast<uint16_t>(ibos.server.Acquire()) };
  Core::CreateIndexBuffer(buffer, flags, ibos[h.id]);
  return h;
}

//=============================================================================
void S::Release(IndexBufferHandle h)
{
  XR_ASSERT(Gfx, h.IsValid());
  Core::Release(h);
}

//=============================================================================
InstanceDataBufferHandle S::CreateInstanceDataBuffer(Buffer const& buffer, InstanceDataStrideType stride)
{
  auto& vbos = sResources->GetVbos();
  InstanceDataBufferHandle h{ static_cast<uint16_t>(vbos.server.Acquire()) };
  Core::CreateInstanceDataBuffer(buffer, stride, vbos[h.id]);
  return h;
}

//=============================================================================
void S::Release(InstanceDataBufferHandle h)
{
  XR_ASSERT(Gfx, h.IsValid());
  Core::Release(h);
}

//=============================================================================
TextureHandle S::CreateTexture(TextureFormat format, uint16_t width,
  uint16_t height, uint16_t depth, FlagType flags, Buffer const* buffers,
  uint8_t numBuffers)
{
  auto& textures = sResources->GetTextures();
  TextureHandle h{ static_cast<uint16_t>(textures.server.Acquire()) };
  auto& ref = textures[h.id];
  ref.inst.info = TextureInfo{ format, width, height, depth,
    TextureInfo::CalculateMipLevels(width, height, flags), flags };
  Core::CreateTexture(buffers, numBuffers, ref);
  return h;
}

//=============================================================================
TextureInfo S::GetTextureInfo(TextureHandle h)
{
  auto& textures = sResources->GetTextures();
  return textures[h.id].inst.info;
}

//=============================================================================
void S::Release(TextureHandle h)
{
  XR_ASSERT(Gfx, h.IsValid());
  Core::Release(h);
}

//=============================================================================
FrameBufferHandle S::CreateFrameBuffer(TextureFormat format, uint16_t width,
  uint16_t height, FlagType flags)
{
  auto& fbos = sResources->GetFbos();
  FrameBufferHandle h{ static_cast<uint16_t>(fbos.server.Acquire()) };
  if (!Core::CreateFrameBuffer(format, width, height, flags, fbos[h.id]))
  {
    fbos.server.Release(h.id);
    h.Invalidate();
  }
  return h;
}

//=============================================================================
FrameBufferHandle S::CreateFrameBuffer(uint8_t textureCount, TextureHandle const* hTextures)
{
  auto& fbos = sResources->GetFbos();
  FrameBufferHandle h{ static_cast<uint16_t>(fbos.server.Acquire()) };
  if (!Core::CreateFrameBuffer(textureCount, hTextures, false, fbos[h.id]))
  {
    fbos.server.Release(h.id);
    h.Invalidate();
  }
  return h;
}

//=============================================================================
FrameBufferHandle S::CreateFrameBuffer(uint8_t textureCount,
  FrameBufferAttachment const* attachments)
{
  auto& fbos = sResources->GetFbos();
  FrameBufferHandle h{ static_cast<uint16_t>(fbos.server.Acquire()) };
  if (!Core::CreateFrameBuffer(textureCount, attachments, false, fbos[h.id]))
  {
    fbos.server.Release(h.id);
    h.Invalidate();
  }
  return h;
}

//=============================================================================
void S::Release(FrameBufferHandle h)
{
  XR_ASSERT(Gfx, h.IsValid());
  Core::Release(h);
}

//=============================================================================
UniformHandle S::CreateUniform(char const* name, UniformType type, uint8_t arraySize)
{
  return sResources->CreateUniform(name, type, arraySize);
}

//=============================================================================
void S::Release(UniformHandle h)
{
  XR_ASSERT(Gfx, h.IsValid());
  sResources->Release(h);
}

//=============================================================================
ShaderHandle S::CreateShader(ShaderType t, Buffer const& buffer)
{
  auto& shaders = sResources->GetShaders();
  ShaderHandle h{ static_cast<uint16_t>(shaders.server.Acquire()) };
  if (!Core::CreateShader(t, buffer, shaders[h.id]))
  {
    shaders.server.Release(h.id);
    h.Invalidate();
  }
  return h;
}

//=============================================================================
void S::Release(ShaderHandle h)
{
  XR_ASSERT(Gfx, h.IsValid());
  Core::Release(h);
}

//=============================================================================
ProgramHandle S::CreateProgram(ShaderHandle hVertex, ShaderHandle hFragment)
{
  auto& programs = sResources->GetPrograms();
  ProgramHandle h{ static_cast<uint16_t>(programs.server.Acquire()) };
  if (!Core::CreateProgram(hVertex, hFragment, programs[h.id]))
  {
    programs.server.Release(h.id);
    h.Invalidate();
  }
  return h;
}

//=============================================================================
void S::Release(ProgramHandle h)
{
  XR_ASSERT(Gfx, h.IsValid());
  Core::Release(h);
}

//=============================================================================
void S::Clear(FlagType flags, Color const& color, float depth, uint8_t stencil)
{
  Core::Clear(flags, color, depth, stencil);
}

//==============================================================================
void S::SetViewport(Rect const& rect)
{
  Core::SetViewport(rect);
}

//==============================================================================
void S::SetScissor(Rect const* rect)
{
  Core::SetScissor(rect);
}

//==============================================================================
void S::SetUniform(UniformHandle h, uint8_t numElems, void const* data)
{
  uint32_t sizeBytes = sResources->GetUniformDataSize(h, numElems);
  Core::SetUniform(h, Buffer{ sizeBytes, static_cast<const uint8_t*>(data) });
}

//==============================================================================
void S::SetTexture(TextureHandle h, uint8_t stage)
{
  XR_ASSERT(Gfx, stage < kMaxTextureStages);
  Core::SetTexture(h, stage);
}

//==============================================================================
void S::SetState(FlagType flags)
{
  Core::SetState(flags);
}

//==============================================================================
void S::SetStencilState(FlagType front, FlagType back)
{
  Core::SetStencilState(front, back);
}

//==============================================================================
void S::SetInstanceData(InstanceDataBufferHandle h, uint32_t offset, uint32_t count)
{
  Core::SetInstanceData(h, offset, count);
}

//==============================================================================
void S::SetProgram(ProgramHandle h)
{
  Core::SetProgram(h);
}

//==============================================================================
void S::SetFrameBuffer(FrameBufferHandle h)
{
  Core::SetFrameBuffer(h);
}

//==============================================================================
void S::Draw(VertexBufferHandle vbh, Primitive primitive, uint32_t offset, uint32_t count)
{
  Core::Draw(vbh, primitive, offset, count);
}

//=============================================================================
void S::Draw(VertexBufferHandle vbh, IndexBufferHandle ibh, Primitive primitive,
  uint32_t offset, uint32_t count)
{
  Core::Draw(vbh, ibh, primitive, offset, count);
}

//==============================================================================
void S::Flush()
{
  Core::Flush();
  Core::OnFlush();
}

//==============================================================================
void S::Present(bool resetState)
{
  Core::Present(resetState);
  Core::OnFlush();
}

//=============================================================================
void S::ReadFrameBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
  TextureFormat format, uint8_t colorAttachment, void* mem,
  ReadFrameBufferCompleteCallback* onComplete)
{
  Core::ReadFrameBuffer(x, y, width, height, format, colorAttachment, mem, onComplete);
  delete onComplete;
}

//==============================================================================
Signal<void>& S::FlushSignal()
{
  return Core::FlushSignal();
}

//==============================================================================
Signal<void>& S::ShutdownSignal()
{
  return Core::ShutdownSignal();
}

//=============================================================================
void S::Shutdown()
{
  Core::OnShutdown();
  Core::Shutdown();
}

} // Gfx
}

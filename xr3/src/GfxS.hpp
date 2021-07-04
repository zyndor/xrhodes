#ifndef XR_GFXIMPL_HPP
#define XR_GFXIMPL_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Gfx.hpp"
#include "xr/types/fundamentals.hpp"

namespace xr
{
namespace Gfx
{

class ResourceManager;

class S // Single threaded API
{
  XR_NONOBJECT_DECL(S)

public: // static
  static void Init(Context* context, ResourceManager* resources);

  static VertexFormatHandle RegisterVertexFormat(VertexFormat const& format);
  static void Release(VertexFormatHandle h);

  static VertexBufferHandle CreateVertexBuffer(VertexFormatHandle hFormat,
    Buffer const& buffer, FlagType flags);
  static void Release(VertexBufferHandle h);

  static IndexBufferHandle CreateIndexBuffer(Buffer const& buffer, FlagType flags);
  static void Release(IndexBufferHandle h);

  static InstanceDataBufferHandle CreateInstanceDataBuffer(Buffer const& buffer,
    InstanceDataStrideType stride);
  static void Release(InstanceDataBufferHandle h);

  static TextureHandle CreateTexture(TextureFormat format, uint16_t width,
    uint16_t height, uint16_t depth, FlagType flags, Buffer const* buffers,
    uint8_t numBuffers);
  static TextureInfo GetTextureInfo(TextureHandle h);
  static void Release(TextureHandle h);

  static FrameBufferHandle CreateFrameBuffer(TextureFormat format, uint16_t width,
    uint16_t height, FlagType flags);
  static FrameBufferHandle CreateFrameBuffer(uint8_t textureCount,
    TextureHandle const* hTextures);
  static FrameBufferHandle CreateFrameBuffer(uint8_t textureCount,
    FrameBufferAttachment const* attachments);
  static void Release(FrameBufferHandle h);

  static UniformHandle CreateUniform(char const* name, UniformType type, uint8_t arraySize);
  static void Release(UniformHandle h);

  static ShaderHandle CreateShader(ShaderType type, Buffer const& buffer);
  static void Release(ShaderHandle h);

  static ProgramHandle CreateProgram(ShaderHandle hVertex, ShaderHandle hFragment);
  static void Release(ProgramHandle h);

  static void Clear(FlagType flags, Color const& color, float depth, uint8_t stencil);

  static void SetViewport(Rect const& rect);
  static void SetScissor(Rect const* rect);
  static void SetUniform(UniformHandle h, uint8_t numElems, void const* data);
  static void SetTexture(TextureHandle h, uint8_t stage);
  static void SetState(FlagType flags);
  static void SetStencilState(FlagType front, FlagType back);
  static void SetInstanceData(InstanceDataBufferHandle h, uint32_t offset, uint32_t count);
  static void SetProgram(ProgramHandle h);
  static void SetFrameBuffer(FrameBufferHandle h);

  static void Draw(VertexBufferHandle vbh, Primitive pt, uint32_t offset, uint32_t count);
  static void Draw(VertexBufferHandle vbh, IndexBufferHandle ibh, Primitive pt,
    uint32_t offset, uint32_t count);
  static void Flush();
  static void Present(bool resetState);

  static void ReadFrameBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
    TextureFormat format, uint8_t colorAttachment, void* mem,
    ReadFrameBufferCompleteCallback* onComplete);

  static Signal<void>& FlushSignal();
  static Signal<void>& ShutdownSignal();

  static void Shutdown();
};

} // Gfx
}

#endif //XR_IGFX_HPP

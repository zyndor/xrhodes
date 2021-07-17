#ifndef XR_GFXCORE_HPP
#define XR_GFXCORE_HPP
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
#include "GfxResourceManager.hpp"

namespace xr
{
namespace Gfx
{

class ResourceManager;

class Core
{
  XR_NONOBJECT_DECL(Core)

public:
  static void Init(Context* context, ResourceManager* resources);

  static void Release(VertexFormatHandle h);

  static void CreateVertexBuffer(VertexFormatHandle hFormat, Buffer const& buffer,
    FlagType flags, VertexBufferObject& vbo);
  static void Release(VertexBufferHandle h);

  static void CreateIndexBuffer(Buffer const& buffer, FlagType flags,
    IndexBufferObject& ibo);
  static void Release(IndexBufferHandle h);

  static void CreateInstanceDataBuffer(Buffer const& buffer, InstanceDataStrideType stride,
    VertexBufferObject& idbo);
  static void Release(InstanceDataBufferHandle h);

  static void CreateTexture(Buffer const* buffers, uint8_t numBuffers, TextureRef& texture);
  static void Release(TextureHandle h);

  static bool CreateFrameBuffer(TextureFormat format, Px width, Px height,
    FlagType flags, FrameBufferObject& fbo);
  static bool CreateFrameBuffer(uint8_t textureCount, TextureHandle const* hTextures,
    bool ownTextures, FrameBufferObject& fbo);
  static bool CreateFrameBuffer(uint8_t textureCount, FrameBufferAttachment const* attachments,
    bool ownTextures, FrameBufferObject& fbo);
  static void Release(FrameBufferHandle h);

  static bool CreateShader(ShaderType type, Buffer const& buffer, ShaderRef& shader);
  static void Release(ShaderHandle h);

  static bool CreateProgram(ShaderHandle hVertex, ShaderHandle hFragment, Program& program);
  static void Release(ProgramHandle h);

  static void Clear(FlagType flags, Color const& color, float depth, uint8_t stencil);

  static void SetViewport(Rect const& rect);
  static void SetScissor(Rect const* rect);
  static void SetUniform(UniformHandle h, Buffer const& buffer);
  static void SetTexture(TextureHandle h, uint8_t stage);
  static void SetState(FlagType flags);
  static void SetStencilState(FlagType front, FlagType back);
  static void SetInstanceData(InstanceDataBufferHandle h, uint32_t offset, uint32_t count);
  static void SetProgram(ProgramHandle h);
  static void SetFrameBuffer(FrameBufferHandle h);

  static void Draw(VertexBufferHandle vbh, Primitive pt, uint32_t offset, uint32_t count);
  static void Draw(VertexBufferHandle vbh, IndexBufferHandle ibh, Primitive pt, uint32_t offset, uint32_t count);
  static void Flush();
  static void Present(bool resetState);

  static void ReadFrameBuffer(Px x, Px y, Px width, Px height,
    TextureFormat format, uint8_t colorAttachment, void* mem,
    ReadFrameBufferCompleteCallback* onComplete);

  static Signal<void>& FlushSignal();
  static void OnFlush();

  static Signal<void>& ShutdownSignal();
  static void OnShutdown();

  static void Shutdown();
};

} // Gfx
}

#endif //XR_GFXCORE_HPP_-

//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "GfxContext.hpp"
#include "GfxM.hpp"
#include "GfxS.hpp"
#include "GfxResourceManager.hpp"
#include "xr/Config.hpp"
#include "xr/memory/memory.hpp"
#include "xr/utility/Hash.hpp"
#include "xr/debug.hpp"
#include <cinttypes>

namespace xr
{
namespace Gfx
{
namespace
{
const uint32_t kDefaultMessageBufferSize = XR_KBYTES(64);

Context* sContext = nullptr;
ResourceManager* sResources = nullptr;

VertexFormatHandle(*sRegisterVertexFormat)(VertexFormat const&) = nullptr;
void(*sReleaseVertexFormat)(VertexFormatHandle h) = nullptr;

VertexBufferHandle(*sCreateVertexBuffer)(VertexFormatHandle hFormat, Buffer const& buffer, FlagType flags) = nullptr;
void(*sReleaseVertexBuffer)(VertexBufferHandle h) = nullptr;

IndexBufferHandle(*sCreateIndexBuffer)(Buffer const& buffer, FlagType flags) = nullptr;
void(*sReleaseIndexBuffer)(IndexBufferHandle h) = nullptr;

InstanceDataBufferHandle(*sCreateInstanceDataBuffer)(Buffer const& buffer, InstanceDataStrideType stride) = nullptr;
void(*sReleaseInstanceDataBuffer)(InstanceDataBufferHandle h) = nullptr;

TextureHandle(*sCreateTexture)(TextureFormat format, uint16_t width,
  uint16_t height, uint16_t depth, FlagType flags, Buffer const* buffer,
  uint8_t numBuffers) = nullptr;

TextureInfo(*sGetTextureInfo)(TextureHandle h) = nullptr;
void(*sReleaseTexture)(TextureHandle h) = nullptr;

FrameBufferHandle(*sCreateFrameBufferWithPrivateTexture)(TextureFormat format, uint16_t width, uint16_t height,
  FlagType flags) = nullptr;
FrameBufferHandle(*sCreateFrameBufferFromTextures)(uint8_t textureCount, TextureHandle const* hTextures) = nullptr;
FrameBufferHandle(*sCreateFrameBufferFromAttachments)(uint8_t textureCount,
  FrameBufferAttachment const* attachments) = nullptr;
void(*sReleaseFrameBuffer)(FrameBufferHandle h) = nullptr;

UniformHandle(*sCreateUniform)(char const* name, UniformType type, uint8_t arraySize) = nullptr;
void(*sReleaseUniform)(UniformHandle h) = nullptr;

ShaderHandle(*sCreateShader)(ShaderType type, Buffer const& buffer) = nullptr;
void(*sReleaseShader)(ShaderHandle h) = nullptr;

ProgramHandle(*sCreateProgram)(ShaderHandle hVertex, ShaderHandle hFragment) = nullptr;
void(*sReleaseProgram)(ProgramHandle h) = nullptr;

void(*sClear)(FlagType flags, Color const& color, float depth, uint8_t stencil) = nullptr;

void(*sSetViewport)(Rect const& rect) = nullptr;
void(*sSetScissor)(Rect const* rect) = nullptr;
void(*sSetUniform)(UniformHandle h, uint8_t elem, void const* data) = nullptr;
void(*sSetTexture)(TextureHandle h, uint8_t stage) = nullptr;
void(*sSetState)(FlagType flags) = nullptr;
void(*sSetStencilState)(FlagType front, FlagType back) = nullptr;
void(*sSetInstanceData)(InstanceDataBufferHandle h, uint32_t offset, uint32_t count) = nullptr;
void(*sSetProgram)(ProgramHandle h) = nullptr;
void(*sSetFrameBuffer)(FrameBufferHandle h) = nullptr;

void(*sDraw)(VertexBufferHandle vbh, Primitive pt, uint32_t offset, uint32_t count) = nullptr;
void(*sDrawIndexed)(VertexBufferHandle vbh, IndexBufferHandle ibh, Primitive pt, uint32_t offset, uint32_t count) = nullptr;
void(*sFlush)() = nullptr;
void(*sPresent)(bool resetState) = nullptr;

void(*sReadFrameBuffer)(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
  TextureFormat format, uint8_t colorAttachment, void* mem,
  ReadFrameBufferCompleteCallback* onComplete) = nullptr;

Signal<void>&(*sFlushSignal)() = nullptr;
Signal<void>&(*sShutdownSignal)() = nullptr;

void(*sShutdown)();

} // nonamespace

//=============================================================================
uint32_t VertexFormat::CalculateHash() const
{
  return Hash::Data32(m_attributes, sizeof(m_attributes) + sizeof(m_offset));
}

//==============================================================================
uint8_t TextureInfo::CalculateMipLevels(uint16_t width, uint16_t height, FlagType flags)
{
  return CheckAllMaskBits(flags, F_TEXTURE_MIPMAP) ?
    static_cast<uint8_t>(std::log2(std::min(width, height))) : 1;
}

//==============================================================================
void  Init(Context* ctx)
{
  XR_ASSERTMSG(Gfx, !sContext, ("Already initialised."));
  XR_ASSERTMSG(Gfx, ctx, ("Can't initialise with empty context."));
  sContext = ctx;

  auto resources = new ResourceManager();
  sResources = resources;

  if (Config::GetInt("XR_GFX_MULTITHREADED", 0) == 1)
  {
    uint32_t commandBufferSize = Config::GetInt("XR_GFX_COMMAND_BUFFER_SIZE",
      kDefaultMessageBufferSize);
    XR_TRACE(Gfx, ("Multithreaded mode; command buffer size: %" PRIu32 " bytes.",
      commandBufferSize));

    M::Init(ctx, resources, commandBufferSize);

#define M_API(x) s##x = M::x
#define M_APIS(x, suffix) s##x##suffix = M::x
    M_API(RegisterVertexFormat);
    M_APIS(Release, VertexFormat);

    M_API(CreateVertexBuffer);
    M_APIS(Release, VertexBuffer);

    M_API(CreateIndexBuffer);
    M_APIS(Release, IndexBuffer);

    M_API(CreateInstanceDataBuffer);
    M_APIS(Release, InstanceDataBuffer);

    M_API(CreateTexture);
    M_API(GetTextureInfo);
    M_APIS(Release, Texture);

    M_APIS(CreateFrameBuffer, WithPrivateTexture);
    M_APIS(CreateFrameBuffer, FromTextures);
    M_APIS(CreateFrameBuffer, FromAttachments);
    M_APIS(Release, FrameBuffer);

    M_API(CreateUniform);
    M_APIS(Release, Uniform);

    M_API(CreateShader);
    M_APIS(Release, Shader);

    M_API(CreateProgram);
    M_APIS(Release, Program);

    M_API(Clear);
    M_API(SetViewport);
    M_API(SetScissor);
    M_API(SetUniform);
    M_API(SetTexture);
    M_API(SetState);
    M_API(SetStencilState);
    M_API(SetInstanceData);
    M_API(SetProgram);
    M_API(SetFrameBuffer);

    M_API(Draw);
    M_APIS(Draw, Indexed);
    M_API(Flush);
    M_API(Present);

    M_API(ReadFrameBuffer);

    M_API(FlushSignal);
    M_API(ShutdownSignal);

    M_API(Shutdown);
#undef M_API
#undef M_APIS
  }
  else
  {
    S::Init(ctx, resources);

#define S_API(x) s##x = S::x
#define S_APIS(x, suffix) s##x##suffix = S::x
    S_API(RegisterVertexFormat);
    S_APIS(Release, VertexFormat);

    S_API(CreateVertexBuffer);
    S_APIS(Release, VertexBuffer);

    S_API(CreateIndexBuffer);
    S_APIS(Release, IndexBuffer);

    S_API(CreateInstanceDataBuffer);
    S_APIS(Release, InstanceDataBuffer);

    S_API(CreateTexture);
    S_API(GetTextureInfo);
    S_APIS(Release, Texture);

    S_APIS(CreateFrameBuffer, WithPrivateTexture);
    S_APIS(CreateFrameBuffer, FromTextures);
    S_APIS(CreateFrameBuffer, FromAttachments);
    S_APIS(Release, FrameBuffer);

    S_API(CreateUniform);
    S_APIS(Release, Uniform);

    S_API(CreateShader);
    S_APIS(Release, Shader);

    S_API(CreateProgram);
    S_APIS(Release, Program);

    S_API(Clear);
    S_API(SetViewport);
    S_API(SetScissor);
    S_API(SetUniform);
    S_API(SetTexture);
    S_API(SetState);
    S_API(SetStencilState);
    S_API(SetInstanceData);
    S_API(SetProgram);
    S_API(SetFrameBuffer);

    S_API(Draw);
    S_APIS(Draw, Indexed);
    S_API(Flush);
    S_API(Present);

    S_API(ReadFrameBuffer);

    S_API(FlushSignal);
    S_API(ShutdownSignal);

    S_API(Shutdown);
#undef CORE_API
#undef CORE_APIS
  }
}

//=============================================================================
int GetLogicalWidth()
{
  return sContext->GetLogicalWidth();
}

//=============================================================================
int GetLogicalHeight()
{
  return sContext->GetLogicalHeight();
}

//=============================================================================
float GetLogicalAspectRatio()
{
  return GetLogicalWidth() / float(GetLogicalHeight());
}

//=============================================================================
int GetPhysicalWidth()
{
  return sContext->GetPhysicalWidth();
}

//=============================================================================
int GetPhysicalHeight()
{
  return sContext->GetPhysicalHeight();
}

//=============================================================================
float GetPhysicalAspectRatio()
{
  return GetPhysicalWidth() / float(GetPhysicalHeight());
}

//==============================================================================
TextureHandle GetDefaultTexture2D()
{
  return TextureHandle(ResourceManager::Textures::kSize - 1);
}

//==============================================================================
TextureHandle GetDefaultTexture3D()
{
  return TextureHandle(ResourceManager::Textures::kSize - 2);
}

//==============================================================================
TextureHandle GetDefaultTextureCube()
{
  return TextureHandle(ResourceManager::Textures::kSize - 3);
}

//==============================================================================
FrameBufferHandle GetDefaultFrameBuffer()
{
  return FrameBufferHandle(ResourceManager::FrameBufferObjects::kSize - 1);
}

//==============================================================================
VertexFormatHandle RegisterVertexFormat(VertexFormat const& format)
{
  return sRegisterVertexFormat(format);
}

//==============================================================================
void Release(VertexFormatHandle h)
{
  if (h.IsValid())
  {
    sReleaseVertexFormat(h);
  }
}

//==============================================================================
VertexBufferHandle CreateVertexBuffer(VertexFormatHandle hFormat, Buffer const& buffer, FlagType flags)
{
  return sCreateVertexBuffer(hFormat, buffer, flags);
}

//==============================================================================
void Release(VertexBufferHandle h)
{
  if (h.IsValid())
  {
    sReleaseVertexBuffer(h);
  }
}

//==============================================================================
IndexBufferHandle CreateIndexBuffer(Buffer const& buffer, FlagType flags)
{
  return sCreateIndexBuffer(buffer, flags);
}

//==============================================================================
void Release(IndexBufferHandle h)
{
  if (h.IsValid())
  {
    sReleaseIndexBuffer(h);
  }
}

//==============================================================================
InstanceDataBufferHandle CreateInstanceDataBuffer(Buffer const& buffer,
  InstanceDataStrideType stride)
{
  return sCreateInstanceDataBuffer(buffer, stride);
}

//==============================================================================
void Release(InstanceDataBufferHandle h)
{
  sReleaseInstanceDataBuffer(h);
}

//==============================================================================
TextureHandle CreateTexture(TextureFormat format, uint16_t width,
  uint16_t height, uint16_t depth, FlagType flags, Buffer const* buffer,
  uint8_t numBuffers)
{
  return sCreateTexture(format, width, height, depth, flags, buffer, numBuffers);
}

//=============================================================================
TextureHandle CreateTexture(TextureFormat format, uint16_t width, uint16_t height,
  uint16_t depth, FlagType flags)
{
  //TODO: XR_ASSERTMSG(Gfx, !kTextureFormats[static_cast<int>(format)].compressed,
  //  ("Cannot create compressed texture without buffer."));
  Buffer buffer{ 0, nullptr };
  return sCreateTexture(format, width, height, depth, flags, &buffer, 1);
}

//==============================================================================
TextureInfo GetTextureInfo(TextureHandle h)
{
  return sGetTextureInfo(h);
}

//==============================================================================
void Release(TextureHandle h)
{
  if (h.IsValid())
  {
    sReleaseTexture(h);
  }
}

//==============================================================================
FrameBufferHandle CreateFrameBuffer(TextureFormat format, uint16_t width, uint16_t height,
  FlagType flags)
{
  return sCreateFrameBufferWithPrivateTexture(format, width, height, flags);
}

//==============================================================================
FrameBufferHandle CreateFrameBuffer(uint8_t textureCount, TextureHandle const* hTextures)
{
  return sCreateFrameBufferFromTextures(textureCount, hTextures);
}

//==============================================================================
FrameBufferHandle CreateFrameBuffer(uint8_t textureCount, FrameBufferAttachment const* attachments)
{
  return sCreateFrameBufferFromAttachments(textureCount, attachments);
}

//==============================================================================
FrameBufferHandle CreateFrameBuffer(uint8_t textureCount, TextureHandle const* hTextures, bool)
{
  return sCreateFrameBufferFromTextures(textureCount, hTextures);
}

//==============================================================================
FrameBufferHandle CreateFrameBuffer(uint8_t textureCount,
  FrameBufferAttachment const* attachments, bool)
{
  return sCreateFrameBufferFromAttachments(textureCount, attachments);
}

//==============================================================================
void Release(FrameBufferHandle h)
{
  if (h.IsValid())
  {
    sReleaseFrameBuffer(h);
  }
}

//==============================================================================
UniformHandle CreateUniform(char const* name, UniformType type, uint8_t arraySize)
{
  return sCreateUniform(name, type, arraySize);
}

//==============================================================================
void Release(UniformHandle h)
{
  if (h.IsValid())
  {
    sReleaseUniform(h);
  }
}

//==============================================================================
ShaderHandle CreateShader(ShaderType type, Buffer const& buffer)
{
  return sCreateShader(type, buffer);
}

//==============================================================================
void Release(ShaderHandle h)
{
  if (h.IsValid())
  {
    sReleaseShader(h);
  }
}

//==============================================================================
ProgramHandle CreateProgram(ShaderHandle hVertex, ShaderHandle hFragment)
{
  return sCreateProgram(hVertex, hFragment);
}

//==============================================================================
void Release(ProgramHandle h)
{
  if (h.IsValid())
  {
    sReleaseProgram(h);
  }
}

//==============================================================================
void Clear(FlagType flags, Color color, float depth, uint8_t stencil)
{
  sClear(flags, color, depth, stencil);
}

//==============================================================================
void SetViewport(Rect const& rect)
{
  sSetViewport(rect);
}

//==============================================================================
void SetScissor(Rect const* rect)
{
  sSetScissor(rect);
}

//==============================================================================
void SetUniform(UniformHandle h, void const* data)
{
  sSetUniform(h, 1, data);
}

//==============================================================================
void SetUniform(UniformHandle h, uint8_t elem, void const* data)
{
  sSetUniform(h, elem, data);
}

//==============================================================================
void SetTexture(TextureHandle h, uint8_t stage)
{
  sSetTexture(h, stage);
}

//==============================================================================
void SetState(FlagType flags)
{
  sSetState(flags);
}

//==============================================================================
void SetStencilState(FlagType front, FlagType back)
{
  sSetStencilState(front, back);
}

//==============================================================================
void SetInstanceData(InstanceDataBufferHandle h, uint32_t offset, uint32_t count)
{
  sSetInstanceData(h, offset, count);
}

//==============================================================================
void SetProgram(ProgramHandle h)
{
  sSetProgram(h);
}

//==============================================================================
void SetFrameBuffer(FrameBufferHandle h)
{
  sSetFrameBuffer(h);
}

//==============================================================================
void Draw(VertexBufferHandle vbh, Primitive pt, uint32_t offset, uint32_t count)
{
  sDraw(vbh, pt, offset, count);
}

//==============================================================================
void Draw(VertexBufferHandle vbh, IndexBufferHandle ibh, Primitive pt, uint32_t offset, uint32_t count)
{
  sDrawIndexed(vbh, ibh, pt, offset, count);
}

//==============================================================================
void Flush()
{
  sFlush();
}

//==============================================================================
void Present(bool resetState)
{
  sPresent(resetState);
}

//==============================================================================
void ReadFrameBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
  TextureFormat format, void* mem, ReadFrameBufferCompleteCallback* onComplete)
{
  if (onComplete)
  {
    onComplete = onComplete->Clone();
  }

  sReadFrameBuffer(x, y, width, height, format, 0, mem, onComplete);
}

//==============================================================================
void ReadFrameBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
  TextureFormat format, uint8_t colorAttachment, void* mem,
  ReadFrameBufferCompleteCallback* onComplete)
{
  if (onComplete)
  {
    onComplete = onComplete->Clone();
  }

  sReadFrameBuffer(x, y, width, height, format, colorAttachment, mem, onComplete);
}

//==============================================================================
Signal<void>& FlushSignal()
{
  return sFlushSignal();
}

//==============================================================================
Signal<void>& ShutdownSignal()
{
  return sShutdownSignal();
}

//==============================================================================
void Shutdown()
{
  XR_ASSERTMSG(Gfx, sContext, ("Shutdown failed: not initialised."));
  sContext = nullptr;

  (*sShutdown)();

  delete sResources;
  sResources = nullptr;

#define API_SHUTDOWN(x) s##x = nullptr
  API_SHUTDOWN(RegisterVertexFormat);
  API_SHUTDOWN(ReleaseVertexFormat);

  API_SHUTDOWN(CreateVertexBuffer);
  API_SHUTDOWN(ReleaseVertexBuffer);

  API_SHUTDOWN(CreateIndexBuffer);
  API_SHUTDOWN(ReleaseIndexBuffer);

  API_SHUTDOWN(CreateInstanceDataBuffer);
  API_SHUTDOWN(ReleaseInstanceDataBuffer);

  API_SHUTDOWN(CreateTexture);
  API_SHUTDOWN(GetTextureInfo);
  API_SHUTDOWN(ReleaseTexture);

  API_SHUTDOWN(CreateFrameBufferWithPrivateTexture);
  API_SHUTDOWN(CreateFrameBufferFromTextures);
  API_SHUTDOWN(CreateFrameBufferFromAttachments);
  API_SHUTDOWN(ReadFrameBuffer);
  API_SHUTDOWN(ReleaseFrameBuffer);

  API_SHUTDOWN(CreateUniform);
  API_SHUTDOWN(ReleaseUniform);

  API_SHUTDOWN(CreateShader);
  API_SHUTDOWN(ReleaseShader);

  API_SHUTDOWN(CreateProgram);
  API_SHUTDOWN(ReleaseProgram);

  API_SHUTDOWN(Clear);
  API_SHUTDOWN(SetViewport);
  API_SHUTDOWN(SetScissor);
  API_SHUTDOWN(SetUniform);
  API_SHUTDOWN(SetTexture);
  API_SHUTDOWN(SetState);
  API_SHUTDOWN(SetStencilState);
  API_SHUTDOWN(SetInstanceData);
  API_SHUTDOWN(SetProgram);
  API_SHUTDOWN(SetFrameBuffer);

  API_SHUTDOWN(Draw);
  API_SHUTDOWN(DrawIndexed);
  API_SHUTDOWN(Flush);
  API_SHUTDOWN(Present);

  API_SHUTDOWN(FlushSignal);
  API_SHUTDOWN(ShutdownSignal);

  API_SHUTDOWN(Shutdown);
#undef API_SHUTDOWN
}

} // Gfx
}

#ifndef XR_GFX_HPP
#define XR_GFX_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Primitive.hpp"
#include "xr/events/Callback.hpp"
#include "xr/events/Signal.hpp"
#include "xr/memory/Buffer.hpp"
#include "xr/math/Rect.hpp"
#include "xr/math/Color.hpp"
#include "xr/debug.hpp"
#include "xr/utils.hpp"
#include <cstring>

namespace xr
{
namespace Gfx
{

class Context;

//=============================================================================
///@brief Upper limit on number of texture stages.
enum : uint8_t { kMaxTextureStages = 8 };

///@brief Upper limit on number of instance data buffers.
enum : uint8_t { kMaxInstanceData = 4 };

//=============================================================================
namespace Comparison
{
using Type = uint8_t;

///@brief These values specify the comparison function used when testing
/// aspects of incoming fragments with that already in the frame buffer.
/// For depth test enabled states, you may use DepthFuncToState() to obtain a
/// bit pattern that can be masked into the value passed into SetState().
/// For stencil test, refer to StencilOp.
enum Value: Type
{
  FAIL = 0x01,
  LESS = 0x02,
  LEQUAL = 0x03,
  EQUAL = 0x04,
  GEQUAL = 0x05,
  GREATER = 0x06,
  NEQUAL = 0x07,
  PASS = 0x08,
};
} // Comparison

//=============================================================================
namespace BlendFactor
{
using Type = uint8_t;

///@brief These values control how rgb / alpha from incoming fragments will be
/// blended with that already in the framebuffer. Use these with
/// BlendFactorToState() to obtain a bit pattern that can be masked into the
/// value passed into SetState().
enum Value
{
  ZERO = 0x01,
  ONE = 0x02,
  SRC_COLOR = 0x03,
  INV_SRC_COLOR = 0x04,
  SRC_ALPHA = 0x05,
  INV_SRC_ALPHA = 0x06,
  DST_COLOR = 0x07,
  INV_DST_COLOR = 0x08,
  DST_ALPHA = 0x09,
  INV_DST_ALPHA = 0x0a,
};
} // BlendFactor

//=============================================================================
namespace StencilOp
{
using Type = uint8_t;

///@brief These values control what's written to the stencil buffer in stencil
/// test enabled states' three scenarios of a fragment 1, failing the test, 2,
/// passing stencil test but failing depth test, and 3, passing both. Use
/// StencilToState() with these to obtain a bit pattern that can be passed to
/// SetStencilState().
enum Value: Type
{
  KEEP = 0x1,
  ZERO = 0x2,
  INCR = 0x3,
  INCR_WRAP = 0x4,
  DECR = 0x5,
  DECR_WRAP = 0x6,
  INVERT = 0x7,
  REPLACE = 0x8,
};
} // StencilOp

//=============================================================================
using FlagType = uint32_t;

enum : FlagType
{
  F_BUFFER_NONE = 0,
  F_BUFFER_INDIRECT_DRAW = XR_MASK_ID(FlagType, 0), // TODO: implement support
  F_BUFFER_INDEX_32BITS = XR_MASK_ID(FlagType, 1),
  F_BUFFER_INSTANCE_DATA = XR_MASK_ID(FlagType, XR_BITSIZEOF(FlagType) - 1),

  F_TEXTURE_NONE = 0,
  F_TEXTURE_WRAP = XR_MASK_ID(FlagType, 0), // clamp is default
  F_TEXTURE_FILTER_POINT = XR_MASK_ID(FlagType, 1), // linear filtering is default
  F_TEXTURE_SRGB = XR_MASK_ID(FlagType, 2),
  F_TEXTURE_MIPMAP_NEAREST = XR_MASK_ID(FlagType, 5),  // trilinear filtering for mipmaps is default; requires F_TEXTURE_MIPMAPS also set;
  F_TEXTURE_MIPMAPS [[deprecated("use F_TEXTURE_MIPMAP")]] = XR_MASK_ID(FlagType, 6),  // no mipmap by default
  F_TEXTURE_MIPMAP = XR_MASK_ID(FlagType, 6),  // no mipmap by default
  F_TEXTURE_CUBE = XR_MASK_ID(FlagType, 7),

  F_STATE_NONE = 0,
  F_STATE_DEPTH_TEST = XR_MASK_ID(FlagType, 0), // off by default
  F_STATE_DEPTH_WRITE = XR_MASK_ID(FlagType, 1), // off by default
  F_STATE_ALPHA_BLEND = XR_MASK_ID(FlagType, 2), // off by default
  F_STATE_CULL_BACK = XR_MASK_ID(FlagType, 3),  // no culling by default
  F_STATE_CULL_FRONT = XR_MASK_ID(FlagType, 4), // only points and lines drawn when F_STATE_CULL_BACK | F_STATE_CULL_FRONT set
  F_STATE_WIREFRAME = XR_MASK_ID(FlagType, 6),
  F_STATE_STENCIL_TEST = XR_MASK_ID(FlagType, 8),

  // Blend factor flags - Use BlendFactorToState() to calculate state flags.
  F_STATE_BLENDF_BASE_SHIFT = 16,
  F_STATE_BLENDF_DST_SHIFT = 8,
  F_STATE_BLENDF_ALPHA_SHIFT = 4,
  F_STATE_BLENDF_MASK = 0xffffu << F_STATE_BLENDF_BASE_SHIFT,

  F_STATE_DEPTH_COMPF_SHIFT = 12,
  F_STATE_DEPTH_COMPF_MASK = 0x0fu << F_STATE_DEPTH_COMPF_SHIFT,

  F_STENCIL_SAME = FlagType(0xffffffff),
  F_STENCIL_REF_MASK            = 0x000000ffu,
  F_STENCIL_MASK_SHIFT          = 8,
  F_STENCIL_MASK_MASK           = 0x0000ff00u,
  F_STENCIL_COMPF_SHIFT         = 16,
  F_STENCIL_COMPF_MASK          = 0x000f0000u,
  F_STENCIL_FAIL_OP_SHIFT       = 20,
  F_STENCIL_FAIL_OP_MASK        = 0x00f00000u,
  F_STENCIL_DEPTH_FAIL_OP_SHIFT = 24,
  F_STENCIL_DEPTH_FAIL_OP_MASK  = 0x0f000000u,
  F_STENCIL_ALL_PASS_OP_SHIFT   = 28,
  F_STENCIL_ALL_PASS_OP_MASK    = 0xf0000000u,
  F_STENCIL_OP_MASK = F_STENCIL_FAIL_OP_MASK | F_STENCIL_DEPTH_FAIL_OP_MASK |
    F_STENCIL_ALL_PASS_OP_MASK,

  F_CLEAR_NONE = 0,
  F_CLEAR_COLOR = XR_MASK_ID(FlagType, 0),
  F_CLEAR_DEPTH = XR_MASK_ID(FlagType, 1),
  F_CLEAR_STENCIL = XR_MASK_ID(FlagType, 2),
};

//=============================================================================
enum class Attribute: uint8_t
{
  Position,
  Normal,
  Tangent,
  Bitangent,
  Color0,
  Color1,
  UV0,
  UV1,
  UV2,
  UV3,
  UV4,
  UV5,
  UV6,
  UV7,

  kCount
};

//=============================================================================
///@brief Describes the format of a vertex buffer with regards to attributes
/// defined, with number of components and whether the value is normalized.
///@note Only floating point types are supported.
class VertexFormat
{
public:
  // structors
  VertexFormat()
  {
    std::memset(m_attributes, 0xff, sizeof(m_attributes));
  }

  // general
  void Add(Attribute attrib, uint8_t numComponents, bool normalize)
  {
    XR_ASSERT(VertexFormat, numComponents > 0 && numComponents <= 4);
    XR_ASSERT(VertexFormat, !Has(attrib));
    const uint8_t iAttrib = uint8_t(attrib);
    // n(ccccc)cc - 1 bit normalized, 7 bits numComponents (2 actually used)
    m_attributes[iAttrib] = (numComponents - 1) | ((normalize ? 1 : 0) << 7);
    m_offset[iAttrib] = m_stride;

    m_stride += sizeof(float) * numComponents;
  }

  bool Has(Attribute attrib) const
  {
    return m_attributes[uint8_t(attrib)] != 0xff;
  }

  bool Get(Attribute attrib, uint8_t& numComponents, bool& normalized, uint16_t& offset) const
  {
    const uint8_t value = m_attributes[uint8_t(attrib)];
    bool success = value != 0xff;
    if (success)
    {
      numComponents = (value & 0x7f) + 1;
      normalized = (value & 0x80) != 0;
      offset = m_offset[uint8_t(attrib)];
    }
    return success;
  }

  uint16_t GetStride() const
  {
    return m_stride;
  }

  uint32_t CalculateHash() const;

private:
  // data
  uint8_t m_attributes[uint8_t(Attribute::kCount)];
  uint16_t m_offset[uint8_t(Attribute::kCount)];
  uint16_t m_stride = 0;
};

//=============================================================================
enum class TextureFormat
{
  // uncompressed
  R8,
  RGB565,
  RGB8,
  RGBA8,
  BGRA8,

  // compressed
  // depth / stencil
  D32,
  D24S8,
  D0S8,

  // etc.
  kCount
};

//=============================================================================
struct TextureInfo
{
  static uint8_t CalculateMipLevels(uint16_t width, uint16_t height, FlagType flags);

  TextureFormat format;
  uint16_t width;
  uint16_t height;
  uint16_t depth;
  uint8_t mipLevels;
  FlagType flags;
};

//=============================================================================
enum class ShaderType: uint8_t
{
  Vertex,
  Fragment
};

//=============================================================================
enum class UniformType: uint8_t
{
  Int1,
  Vec4,
  Mat3,
  Mat4,

  kCount
};

//=============================================================================
class HandleCoreCore
{
public:
  enum : uint16_t { INVALID_ID = 0xffff };

  uint16_t id;

  HandleCoreCore(uint16_t id_)
  : id{ id_ }
  {}

  bool IsValid() const
  {
    return id != INVALID_ID;
  }

  void Invalidate()
  {
    id = INVALID_ID;
  }
};

template <typename T>
struct HandleCore : HandleCoreCore
{
  HandleCore(uint16_t id_)
  : HandleCoreCore(id_)
  {}

  bool operator==(T const& rhs) const { return id == rhs.id; }
  bool operator!=(T const& rhs) const { return !operator==(rhs); }
  bool operator<(T const& rhs) const { return id < rhs.id; }
};

#define GFX_HANDLE_DECL(name) struct name: HandleCore<name> \
  { \
    name(uint16_t id_ = HandleCoreCore::INVALID_ID) \
    : HandleCore<name>(id_) \
    {}  \
  };

GFX_HANDLE_DECL(VertexFormatHandle);
GFX_HANDLE_DECL(VertexBufferHandle);
GFX_HANDLE_DECL(IndexBufferHandle);
GFX_HANDLE_DECL(InstanceDataBufferHandle);
GFX_HANDLE_DECL(TextureHandle);
GFX_HANDLE_DECL(FrameBufferHandle);
GFX_HANDLE_DECL(ShaderHandle);
GFX_HANDLE_DECL(ProgramHandle);
GFX_HANDLE_DECL(UniformHandle);
#undef GFX_HANDLE_DECL

//=============================================================================
struct FrameBufferAttachment
{
  TextureHandle hTexture;
  uint8_t mipLevel;
  uint8_t side;
};

//=============================================================================
///@brief Calculates the blend factor from the given BlendFactor values for
/// separate source RGB and alpha, and destination RGB and alpha.
constexpr FlagType BlendFactorToState(BlendFactor::Value srcRgb,
  BlendFactor::Value srcA, BlendFactor::Value dstRgb, BlendFactor::Value dstA)
{
  return (FlagType(srcRgb) << F_STATE_BLENDF_BASE_SHIFT) |
    (FlagType(srcA) << (F_STATE_BLENDF_BASE_SHIFT + F_STATE_BLENDF_ALPHA_SHIFT)) |
    (FlagType(dstRgb) << (F_STATE_BLENDF_BASE_SHIFT + F_STATE_BLENDF_DST_SHIFT)) |
    (FlagType(dstA) << (F_STATE_BLENDF_BASE_SHIFT + F_STATE_BLENDF_DST_SHIFT +
      F_STATE_BLENDF_ALPHA_SHIFT));
}

///@brief Calculates the blend factor from the given BlendFactor values for
/// source RGBA and destination RGBA.
constexpr FlagType BlendFactorToState(BlendFactor::Value srcRgba,
  BlendFactor::Value dstRgba)
{
  return BlendFactorToState(srcRgba, srcRgba, dstRgba, dstRgba);
}

///@brief Calculates a state flag for a given depth comparitor @a depthFunc,
/// which is one of the F_COMPF_* values.
constexpr FlagType DepthFuncToState(Comparison::Value depthFunc)
{
  return FlagType(depthFunc) << F_STATE_DEPTH_COMPF_SHIFT;
}

///@brief Calculates a stencil state flag for the given reference value @a ref,
/// @a mask, and operations which are one of the F_STENCILOP_* values.
///@param ref Stencil reference value.
///@param mask Stencil mask value.
///@param comp Comparison function for source and destination stencil value.
///@param sFail Operation for when the stencil test fails.
///@param dFail Operation for when the stencil test passes, but depth test fails.
/// (Ignored when the depth test is disabled).
///@param sdPass Operation for when both stencil and depth tests pass. (Disabled
/// depth test is assumed to always pass.)
constexpr FlagType StencilToState(uint8_t ref, uint8_t mask,
  Comparison::Value comp, StencilOp::Value sFail, StencilOp::Value dFail,
  StencilOp::Value sdPass)
{
  return FlagType(ref) | (FlagType(mask) << F_STENCIL_MASK_SHIFT) |
    (FlagType(comp) << F_STENCIL_COMPF_SHIFT) |
    (FlagType(sFail) << F_STENCIL_FAIL_OP_SHIFT) |
    (FlagType(dFail) << F_STENCIL_DEPTH_FAIL_OP_SHIFT) |
    (FlagType(sdPass) << F_STENCIL_ALL_PASS_OP_SHIFT);
}

//=============================================================================
///@brief Initialises Gfx with the given Gfx::Context, which the client should
/// get from Device.<br />
/// Gfx supports multithreaded rendering. When this is enabled, render commands
/// are queued in a buffer, which the renderer processes when the pipeline was
/// Flush()ed or a frame was Present()ed.<br/>
/// Multithreaded rendering may be configured using the following options:<br />
/// XR_GFX_MULTITHREADED: 0 (off, default) / 1 (on).
/// XR_GFX_COMMAND_BUFFER_SIZE: size of command buffer in bytes. Default is 64KB.
///@note Multithreaded rendering may be useful when you find your game update
/// expensive; otherwise it is not guaranteed to perform better than single
/// threaded.
///@note Gfx APIs must still only be called from a single thread.
void Init(Context* context);

///@return Logical width of rendering area in pixels.
int  GetLogicalWidth();

///@return Logical height of rendering area in pixels.
int  GetLogicalHeight();

///@return The ratio of the width to the height of the logical size.
float GetLogicalAspectRatio();

///@return Physical size of rendering area in pixels.
int GetPhysicalWidth();

///@return Physical size of rendering area in pixels.
int GetPhysicalHeight();

///@return The ratio of the width to the height of the physical size.
float GetPhysicalAspectRatio();

// default textures.
///@return Handle to the default 2D texture.
///@note Does not transfer ownership; don't try to Release() it.
///@note Returns a constant value. Always synchronous.
TextureHandle GetDefaultTexture2D();

///@return Handle to the default 3D texture.
///@note Does not transfer ownership; don't try to Release() it.
///@note Returns a constant value. Always synchronous.
TextureHandle GetDefaultTexture3D();

///@return Handle to the default cubemap texture.
///@note Does not transfer ownership; don't try to Release() it.
///@note Returns a constant value. Always synchronous.
TextureHandle GetDefaultTextureCube();

///@return Handle to the default framebuffer.
///@note The front and back buffers are color attachments 0 and 1, respectively.
///@note The texture format of the attachments is RGBA8.
///@note Does not transfer ownership; don't try to Release() it.
///@note Returns a constant value. Always synchronous.
FrameBufferHandle GetDefaultFrameBuffer();

///@brief Registers a vertex format definition.
VertexFormatHandle RegisterVertexFormat(VertexFormat const& format);

///@brief Signifies the end of ownership of a vertex format, destroying it when
/// no other owners were left.
void Release(VertexFormatHandle h);

///@brief Creates a vertex buffer with the given format, data and
/// options. Shares ownership of @a hFormat.
VertexBufferHandle  CreateVertexBuffer(VertexFormatHandle hFormat,
  Buffer const& buffer, FlagType flags = F_BUFFER_NONE);

///@brief Signifies the end of ownership of a vertex buffer, destroying it if
/// no other owners were left.
void Release(VertexBufferHandle h);

///@brief Creates an instance data buffer.
InstanceDataBufferHandle  CreateInstanceDataBuffer(Buffer const& buffer,
  uint16_t stride);

///@brief Signifies the end of ownership of an instance data buffer, destroying
/// it if no other owners were left.
void Release(InstanceDataBufferHandle h);

///@brief Creates an index buffer with the given format, data and
/// options.
IndexBufferHandle  CreateIndexBuffer(Buffer const& buffer, FlagType flags = F_BUFFER_NONE);

///@brief Signifies the end of ownership of an index buffer, destroying it if
/// no other owners were left.
void Release(IndexBufferHandle h);

///@brief Creates a texture with the given parameters and texel data in @a buffer.
/// Mipmap levels may be provided if the F_TEXTURE_MIPMAP flag is set; the buffer
/// layout should be per level (highest to lowest), and within that, faces (1 for
/// 2D, 6 for cubemaps, in +x, -x, +y ... order).
///@note The texel data isn't kept around by Gfx.
///@brief The texture will not be active yet; use SetTexture().
TextureHandle CreateTexture(TextureFormat format, uint16_t width,
  uint16_t height, uint16_t depth, FlagType flags, Buffer const* buffer,
  uint8_t numBuffers = 1);

///@brief Creates a texture width the given parameters and no texel data. Such
/// a texture may be suitable for a render target (framebuffer attachment).
///@note @a format may only be one of the non-compressed ones.
///@brief The texture will not be active yet; use SetTexture().
TextureHandle CreateTexture(TextureFormat format, uint16_t width,
  uint16_t height, uint16_t depth, FlagType flags);

///@return Description of the texture that @a h is for.
TextureInfo GetTextureInfo(TextureHandle h);

///@brief Signifies the end of ownership of a texture, destroying it if
/// no other owners were left.
///@note Framebuffers' use of a texture also increments its refcount.
void Release(TextureHandle h);

///@brief Creates a render target along with a 2D texture of the given format.
/// Such a frame buffer may be suitable for use with ReadFrameBuffer() only,
/// since there is no way to access the texture handle of its attachment.
///@note The frame buffer will not be active yet; use SetFrameBuffer().
FrameBufferHandle  CreateFrameBuffer(TextureFormat format, uint16_t width,
  uint16_t height, FlagType flags);

///@brief Creates a render target, attaching the given @a hTextures. The framebuffer
/// retains ownership of the textures in the @a hTextures array, i.e. the caller
/// is required to Release() them once they're no longer needed.
///@note The frame buffer will not be active yet; use SetFrameBuffer().
FrameBufferHandle  CreateFrameBuffer(uint8_t textureCount,
  TextureHandle const* hTextures);

///@brief Creates a render target with the given @a attachments. The framebuffer
/// retains ownership of the textures in the @a attachments, i.e. the caller is
/// required to Release() them once they're no longer needed.
///@note The frame buffer will not be active yet; use SetFrameBuffer().
FrameBufferHandle  CreateFrameBuffer(uint8_t textureCount,
  FrameBufferAttachment const* attachments);

///@brief Creates a render target, attaching the given @a hTextures. If
/// @a ownTextures is set, the render target assumes ownership, i.e.
/// the refcount of the textures is not incremented (but will be
/// decremented when the render target is destroyed).
///@note The frame buffer will not be active yet; use SetFrameBuffer().
[[deprecated("ownTextures is ignored, use the overload without it.")]]
FrameBufferHandle  CreateFrameBuffer(uint8_t textureCount,
  TextureHandle const* hTextures, bool ownTextures);

///@brief Creates a render target with the given @a attachments. If
/// @a ownTextures is set, the render target assumes ownership, i.e.
/// the refcount of the textures is not incremented (but will be
/// decremented when the render target is destroyed).
///@note The frame buffer will not be active yet; use SetFrameBuffer().
[[deprecated("ownTextures is ignored, use the overload without it.")]]
FrameBufferHandle  CreateFrameBuffer(uint8_t textureCount,
  FrameBufferAttachment const* attachments, bool ownTextures);

///@brief Deletes framebuffer, Release()ing all attached textures.
void Release(FrameBufferHandle h);

///@brief Creates a Uniform, to be recognized in shaders that are created following
/// this call. This means that a uniform of a given name should always have the
/// same type and array size across the board.
///@note The maximum array size is 255.
///@note If a uniform with the given name (and matching type and size)
/// already exists, its reference count is incremented instead.
UniformHandle CreateUniform(char const* name, UniformType type, uint8_t arraySize = 1);

///@brief Signifies the end of ownership of a uniform, destroying it if
/// no other owners were left.
///@note Programs' usage of a uniform also increments its refCount.
void Release(UniformHandle h);

///@brief Compiles preprocessed shader. Source isn't kept around.
///@return Handle, if the shader compilation was successful. Invalid handle
/// if not.
ShaderHandle CreateShader(ShaderType t, Buffer const& buffer);

///@brief Signifies the end of ownership of a shader, destroying it if
/// no other owners were left.
///@note Programs' usage of a shader also increments its refcount.
void Release(ShaderHandle h);

///@brief Creates and links a shader program from the given vertex
/// and fragment shaders.
///@return Handle, if linking was successful. Invalid handle if not.
ProgramHandle CreateProgram(ShaderHandle hVertex, ShaderHandle hFragment);

///@brief Destroys the given program and Release()s the shaders and uniforms that
/// it uses.
void Release(ProgramHandle h);

// State and drawing
///@brief Clears the given buffers of the currently bound frame buffer to the
/// specified values.
void Clear(FlagType flags, Color color = Color(0xff000000), float depth = 1.0f,
  uint8_t stencil = 0x00);

///@brief Sets the viewport.
void SetViewport(Rect const& rect);

///@brief Sets rectangle for scissor testing; if null, disables scissor test.
void SetScissor(Rect const* rect);

///@brief Sets a value for the given uniform('s first element, if it is an array).
void SetUniform(UniformHandle h, void const* data);

///@brief Sets a value for the given uniform array's first @a numElems elements.
void SetUniform(UniformHandle h, uint8_t numElems, void const* data);

///@brief Binds the given texture for a texture stage.
void SetTexture(TextureHandle h, uint8_t stage = 0);

///@brief Sets render state. See the F_STATE_* flags and also F_BLENDF_* values
/// and BlendFactorToState().
///@note If blend factor state flags were not set, the blend function is not
/// updated. Default is source RGBA * source alpha, destination RGBA * (1 -
/// source alpha).
void SetState(FlagType flags = F_STATE_NONE);

///@brief Sets the stenciling state for @a front and @a back facing primitives.
/// Passing F_STENCIL_SAME as the value for @a back will copy the settings of
/// @a front.
void SetStencilState(FlagType front, FlagType back = F_STENCIL_SAME);

///@brief Sets instance data to be used for the next Draw call only.
///@note Flush() / Present() does not affect this; instance data set with this
/// method is only consumed by a subsequent Draw() call.
void SetInstanceData(InstanceDataBufferHandle h, uint32_t offset, uint32_t count);

///@brief Sets a shader program to be used for the subsequent Draw calls.
void SetProgram(ProgramHandle h);

///@brief Sets a frame buffer that subsequent Draw() calls will to render to.
void SetFrameBuffer(FrameBufferHandle h);

///@brief Draws the contents of the given vertex buffer; @a count vertices
/// starting from @a offset.
void Draw(VertexBufferHandle vbh, Primitive primitive, uint32_t offset, uint32_t count);

///@brief Draws the contents of the given vertex buffer, using @a count indices
/// starting from @a offset.
void Draw(VertexBufferHandle vbh, IndexBufferHandle ibh, Primitive primitive, uint32_t offset, uint32_t count);

///@brief Flushes the pipeline and calls all callbacks registered on FlushSignal().
///@note In multithreaded mode, this will start processing the commands issued
/// thus far (including this one).
void Flush();

///@brief Flushes the pipeline and performs the swapping of the front and back
/// buffers.
///@note In multithreaded mode, this will start processing the commands issued
/// thus far (including this one).
void Present(bool resetState = true);

///@brief Callback to call upon the completion of a ReadFrameBuffer command.
/// Its void* parameter is the chunk of memory that was passed to ReadFrameBuffer.
using ReadFrameBufferCompleteCallback = Callback<void, void*>;

///@brief Reads the contents of the first colour attachment of the currently
/// set framebuffer, within the boundaries of the given rectangle and in the
/// given format.<br />
/// If provided, @a onComplete will be called upon completion, passing @a mem.
///@note The buffer pointed to by @a mem must be large enough to hold
/// the data.
///@note In multithreaded mode, this will start processing the commands issued
/// thus far (including this one).
///@note Does not transfer ownership of @a onComplete (if any); it will be CLone()d.
///@note @a onComplete (if any) will be called from the rendering thread.
///@note In multithreaded mode, client code must guarantee at least two flushes
/// before attempting to access the result; it's recommended to use the
/// completion callback instead.
void ReadFrameBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
  TextureFormat format, void* mem,
  ReadFrameBufferCompleteCallback* onComplete = nullptr);

///@brief Reads the contents of the given @a colourAttachment of the currently
/// set framebuffer.<br />
/// If provided, @a onComplete will be called upon completion, passing @a mem.
///@note The buffer pointed to by @a mem must be large enough to hold
/// the data.
///@note In multithreaded mode, this will start processing the commands issued
/// thus far (including this one).
///@note Does not transfer ownership of @a onComplete (if any); it will be CLone()d.
///@note @a onComplete (if any) will be called from the rendering thread.
///@note In multithreaded mode, client code must guarantee at least two flushes
/// before attempting to access the result; it's recommended to use the
/// completion callback instead.
void ReadFrameBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
  TextureFormat format, uint8_t colorAttachment, void* mem,
  ReadFrameBufferCompleteCallback* onComplete = nullptr);

///@return A signal which is emitted upon Flush().
///@note In multithreaded mode, this means the issuing of a Flush command, not
/// the execution.
Signal<void>& FlushSignal();

///@brief A signal which is emitted upon Shutdown().
Signal<void>& ShutdownSignal();

///@brief shuts down the renderer and calls all callbacks registere on
/// ShutdownSignal().
void Shutdown();

} // Gfx
}

#endif // XR_GFX_HPP

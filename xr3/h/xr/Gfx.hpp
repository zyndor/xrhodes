#ifndef XR_GFX_HPP
#define XR_GFX_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/Color.hpp"
#include "xr/utils.hpp"
#include "xr/Rect.hpp"
#include "xr/Callback.hpp"
#include "xr/Buffer.hpp"
#include "xr/Primitive.hpp"
#include "xr/debug.hpp"

namespace xr
{
namespace Gfx
{

class Context;

//=============================================================================
namespace Comparison
{
using Type = uint8_t;

///@brief These values specify the comparison function used when testing
/// aspects of incoming fragments with that already in the frame buffer.
/// For depth test enabled states, you may use DepthFuncToState() to obtain a
/// bit pattern that can be masked into the value passed into SetState().
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
  F_TEXTURE_MIPMAPS = XR_MASK_ID(FlagType, 6),  // no mipmaps by default
  F_TEXTURE_CUBE = XR_MASK_ID(FlagType, 7),

  F_STATE_NONE = 0,
  F_STATE_DEPTH_TEST = XR_MASK_ID(FlagType, 0), // off by default
  F_STATE_DEPTH_WRITE = XR_MASK_ID(FlagType, 1), // off by default
  F_STATE_ALPHA_BLEND = XR_MASK_ID(FlagType, 2), // off by default
  F_STATE_CULL_BACK = XR_MASK_ID(FlagType, 3),  // no culling by default
  F_STATE_CULL_FRONT = XR_MASK_ID(FlagType, 4), // only points and lines drawn when F_STATE_CULL_BACK | F_STATE_CULL_FRONT set
  F_STATE_SCISSOR_TEST = XR_MASK_ID(FlagType, 5),
  F_STATE_WIREFRAME = XR_MASK_ID(FlagType, 6),

  // Blend factor flags - Use BlendFactorToState() to calculate state flags.
  F_STATE_BLENDF_BASE_SHIFT = 16,
  F_STATE_BLENDF_DST_SHIFT = 8,
  F_STATE_BLENDF_ALPHA_SHIFT = 4,
  F_STATE_BLENDF_MASK = 0xffffu << F_STATE_BLENDF_BASE_SHIFT,

  F_STATE_DEPTH_COMPF_SHIFT = 12,
  F_STATE_DEPTH_COMPF_MASK = 0x0fu << F_STATE_DEPTH_COMPF_SHIFT,

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
  TextureFormat format = TextureFormat::kCount;
  uint16_t width = 0;
  uint16_t height = 0;
  uint16_t depth = 0;
  uint8_t mipCount = 0;
  FlagType flags = F_TEXTURE_NONE;
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
struct HandleCoreCore {
  enum : uint16_t { INVALID_ID = 0xffff };

  uint16_t id;

  HandleCoreCore()
  : id{ INVALID_ID }
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
  bool operator==(T const& rhs) const { return id == rhs.id; }
  bool operator!=(T const& rhs) const { return !operator==(rhs); }
  bool operator<(T const& rhs) const { return id < rhs.id; }
};

#define GFX_HANDLE_DECL(name) struct name: HandleCore<name> {}
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

//=============================================================================
///@brief Initialises Gfx with the given Gfx::Context, which the client should
/// get from Device. Depending on implementation, this may be called from a
/// thread that you intend to use as your rendering thread, and in one such
/// scenario - still depending on implementation -, 1, the main thread should
/// wait for the initialization to complete, and 2, you are expected to make
/// all subsequent Gfx calls from the same thread Init() was called from.
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

///@brief Registers a vertex format definition.
VertexFormatHandle RegisterVertexFormat(VertexFormat const& format);
void Release(VertexFormatHandle h);

///@brief Creates a vertex buffer with the given format, data and
/// options.
VertexBufferHandle  CreateVertexBuffer(VertexFormatHandle hFormat,
  Buffer const& buffer, FlagType flags = F_BUFFER_NONE);

void Release(VertexBufferHandle h);

///@brief Creates an instance data buffer.
InstanceDataBufferHandle  CreateInstanceDataBuffer(Buffer const& buffer,
  uint16_t stride);

void Release(InstanceDataBufferHandle h);

///@brief Creates an index buffer with the given format, data and
/// options.
IndexBufferHandle  CreateIndexBuffer(Buffer const& buffer, FlagType flags = F_BUFFER_NONE);

void Release(IndexBufferHandle h);

///@brief Creates a texture with the given parameters and texel data in @a buffer.
///@note The texel data isn't kept around by Gfx.
TextureHandle CreateTexture(TextureFormat format, uint32_t width,
  uint32_t height, uint32_t depth, FlagType flags, Buffer const* buffer,
  size_t numBuffers = 1);

///@brief Creates a texture width the given parameters and no texel data. Such
/// a texture may be suitable for a render target (framebuffer attachment).
///@note @a format may only be one of the non-compressed ones.
TextureHandle CreateTexture(TextureFormat format, uint32_t width,
  uint32_t height, uint32_t depth, FlagType flags);

// default textures.
TextureHandle GetDefaultTexture2D();
TextureHandle GetDefaultTexture3D();
TextureHandle GetDefaultTextureCube();

TextureInfo GetTextureInfo(TextureHandle h);

///@brief Decrements the reference count of a texture, and if it has reached 0
/// zero, deletes it.
void Release(TextureHandle h);

///@return Handle to the default framebuffer. The front and back buffers are color
/// attachments 0 and 1.
FrameBufferHandle GetDefaultFrameBuffer();

///@brief Creates a render target along with a 2D texture of the given format.
/// Such a frame buffer may be suitable for use with ReadFrameBuffer() only,
/// since there is no way to access its attachments' texture handle.
FrameBufferHandle  CreateFrameBuffer(TextureFormat format, uint32_t width,
  uint32_t height, FlagType flags);

///@brief Creates a render target, attaching the given @a hTextures. If
/// @a ownTextures is set, the render target assumes ownership, i.e.
/// the refcount of the textures is not incremented (but will be
/// decremented when the render target is destroyed).
FrameBufferHandle  CreateFrameBuffer(uint8_t textureCount,
  TextureHandle const* hTextures, bool ownTextures);

///@brief Creates a render target with the given @a attachments. If
/// @a ownTextures is set, the render target assumes ownership, i.e.
/// the refcount of the textures is not incremented (but will be
/// decremented when the render target is destroyed).
FrameBufferHandle  CreateFrameBuffer(uint8_t textureCount,
  FrameBufferAttachment const* attachments, bool ownTextures);

///@brief Reads the content of the first colour attachment of the currently
/// set framebuffer, within the boundaries of the given rectangle and in the
/// given format. The texture format of the default framebuffer is RGBA8.
///@note The buffer pointed to by @a mem must be large enough to hold
/// the data.
void ReadFrameBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
  TextureFormat format, void* mem);

///@brief Reads given colour attachment of currently set framebuffer; if
/// this is the default one then the contents of the front or back
/// buffers may be read by specifying attachments 0 or 1, respectively.
void ReadFrameBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
  TextureFormat format, uint16_t colorAttachment, void* mem);

///@brief Deletes framebuffer, decrementing the refcount of all attached textures.
void Release(FrameBufferHandle h);

///@brief Creates a Uniform, to be recognized in shaders that are created following
/// this call. This means that a uniform of a given name should always have the
/// same type and array size across the board. Note that the maximum array size
/// is 255.
///@note If a uniform with the given name (and matching type and size)
/// already exists, its reference count is incremented instead.
UniformHandle CreateUniform(char const* name, UniformType type, uint8_t arraySize = 1);

///@brief Decrements the reference count of a uniform, and if it has reached 0,
/// destroys it.
///@note Programs' usage of a uniform also increments its refCount.
void Release(UniformHandle h);

///@brief Compiles preprocessed shader. Source isn't kept around.
///@return Handle, if the shader compilation was successful. INVALID_HANDLE
/// if not.
ShaderHandle CreateShader(ShaderType t, Buffer const& buffer);

///@brief Decrements the reference count of the shader and if it has reached 0,
/// destroys it.
void Release(ShaderHandle h);

///@brief Creates and links a shader program from the given vertex
/// and fragment shaders.
///@return Handle, if linking was successful. INVALID_HANDLE if not.
ProgramHandle CreateProgram(ShaderHandle hVertex, ShaderHandle hFragment);

///@brief Destroys the given program and releases the shaders and uniforms that
/// it uses.
void Release(ProgramHandle h);

// State and drawing
///@brief Sets the viewport.
void SetViewport(Rect const& rect);

///@brief Sets rectangle for scissor testing; if null, disables scissor test.
void SetScissor(Rect const* rect);

///@brief Sets a value for the given uniform('s first element, if it is an array).
void SetUniform(UniformHandle h, void const* data);

///@brief Sets a value for the given uniform array's first @a numElems elements.
void SetUniform(UniformHandle h, uint8_t numElems, void const* data);

///@brief Binds the given texture for a texture stage.
void SetTexture(TextureHandle h, uint32_t stage = 0);

///@brief Sets render state. See the F_STATE_* flags and also F_BLENDF_* values
/// and BlendFactorToState().
///@note If blend factor state flags were not set, the blend function is not
/// updated. Default is source RGBA * source alpha, destination RGBA * (1 -
/// source alpha).
void SetState(FlagType flags = F_STATE_NONE);

///@brief Sets instance data to be used for the subsequent Draw calls.
void SetInstanceData(InstanceDataBufferHandle h, uint16_t offset, uint16_t count);

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

///@brief Clears the given buffers of the currently bound frame buffer to the
/// specified values.
void Clear(FlagType flags, Color color = Color(0xff000000), float depth = 1.0f,
  uint8_t stencil = 0x00);

///@brief Flushes the pipeline and calls all callbacks registered with
/// RegisterFlushCallback().
void Flush();

///@brief Flushes the pipeline and performs the swapping of the front and back
/// buffers.
void Present(bool resetState = true);

///@brief Registers a function to be called upon Flush().
void RegisterFlushCallback(Callback fn, void* userData);

///@brief Registers a function to be called upon Shutdown().
void RegisterShutdownCallback(Callback fn, void* userData);

///@brief shuts down the renderer.
void Shutdown();

} // GFX
} // XR

#endif // XR_GFX_HPP

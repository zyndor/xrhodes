#ifndef XR_GFX_HPP
#define XR_GFX_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Color.hpp"
#include "XR/memory.hpp"
#include "XR/utils.hpp"
#include "XR/Hash.hpp"
#include "XR/Rect.hpp"
#include "XR/Callback.hpp"
#include "XR/PrimType.hpp"
#include "XR/Buffer.hpp"
#include "XR/debug.hpp"

namespace XR
{
namespace Gfx
{

//=============================================================================
enum Flags: uint32_t
{
  F_BUFFER_NONE = 0,
  F_BUFFER_INDIRECT_DRAW = XR_MASK_ID(uint32_t, 0), // TODO: implement support
  F_BUFFER_INDEX_32BITS = XR_MASK_ID(uint32_t, 1),
  F_BUFFER_INSTANCE_DATA = XR_MASK_ID(uint32_t, XR_BITSIZEOF(Flags) - 1),

  F_TEXTURE_NONE = 0,
  F_TEXTURE_WRAP = XR_MASK_ID(uint32_t, 0), // clamp is default
  F_TEXTURE_FILTER_POINT = XR_MASK_ID(uint32_t, 1), // linear filtering is default
  F_TEXTURE_SRGB = XR_MASK_ID(uint32_t, 2),
  F_TEXTURE_MIPMAPS = XR_MASK_ID(uint32_t, 6),  // no mipmaps by default
  F_TEXTURE_CUBE = XR_MASK_ID(uint32_t, 7),

  F_STATE_NONE = 0,
  F_STATE_DEPTH_TEST = XR_MASK_ID(uint32_t, 0), // off by default
  F_STATE_DEPTH_WRITE = XR_MASK_ID(uint32_t, 1), // off by default
  F_STATE_ALPHA_BLEND = XR_MASK_ID(uint32_t, 2), // off by default
  F_STATE_CULL_BACK = XR_MASK_ID(uint32_t, 3),  // no culling by default
  F_STATE_CULL_FRONT = XR_MASK_ID(uint32_t, 4), // only points and lines drawn when F_STATE_CULL_BACK | F_STATE_CULL_FRONT set
  F_STATE_SCISSOR_TEST = XR_MASK_ID(uint32_t, 5),
  F_STATE_WIREFRAME = XR_MASK_ID(uint32_t, 6),

  F_CLEAR_NONE = 0,
  F_CLEAR_COLOR = XR_MASK_ID(uint32_t, 0),
  F_CLEAR_DEPTH = XR_MASK_ID(uint32_t, 1),
  F_CLEAR_STENCIL = XR_MASK_ID(uint32_t, 2),
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

  uint32_t CalculateHash() const
  {
    return XR::Hash::Data32(m_attributes, sizeof(m_attributes) + sizeof(m_offset));
  }

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
  uint32_t flags = F_TEXTURE_NONE;
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
///@brief Initialises Gfx with the given window handle, which must match the
/// implementation of Device.
void Init(void* window);

///@return Logical width of screen in pixels.
uint16_t GetWidth();

///@return Logical height of screen in pixels.
uint16_t GetHeight();

///@brief Registers a vertex format definition.
VertexFormatHandle RegisterVertexFormat(VertexFormat const& format);
void Destroy(VertexFormatHandle h);

///@brief Creates a vertex buffer with the given format, data and
/// options.
VertexBufferHandle  CreateVertexBuffer(VertexFormatHandle hFormat,
  Buffer const& buffer, uint32_t flags = F_BUFFER_NONE);

void Destroy(VertexBufferHandle h);

///@brief Creates an instance data buffer.
InstanceDataBufferHandle  CreateInstanceDataBuffer(Buffer const& buffer,
  uint16_t stride);

void Destroy(InstanceDataBufferHandle h);

///@brief Creates an index buffer with the given format, data and
/// options.
IndexBufferHandle  CreateIndexBuffer(Buffer const& buffer, uint32_t flags = F_BUFFER_NONE);

void Destroy(IndexBufferHandle h);

///@brief Creates a texture with the given parameters and texel data in @a buffer.
///@note The texel data isn't kept around by Gfx.
TextureHandle CreateTexture(TextureFormat format, uint32_t width,
  uint32_t height, uint32_t depth, uint32_t flags, Buffer const* buffer,
  size_t numBuffers = 1);

///@brief Creates a texture width the given parameters and no texel data. Suxh
/// a texture may be suitable for a render target (framebuffer attachment).
///@note @a format may only be one of the non-compressed ones.
TextureHandle CreateTexture(TextureFormat format, uint32_t width,
  uint32_t height, uint32_t depth, uint32_t flags);

// default textures.
TextureHandle GetDefaultTexture2D();
TextureHandle GetDefaultTexture3D();
TextureHandle GetDefaultTextureCube();

TextureInfo GetTextureInfo(TextureHandle h);

///@brief Decrements refcount of a texture, and if it has reached 0 zero,
/// deletes it.
void Destroy(TextureHandle h);

///@brief Creates a render target, attaching the given textures.
FrameBufferHandle  CreateFrameBuffer(TextureFormat format, uint32_t width,
  uint32_t height, uint32_t flags);

// default framebuffer. The front and back buffers are color attachments 0 and 1.
FrameBufferHandle GetDefaultFrameBuffer();

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
void Destroy(FrameBufferHandle h);

///@brief Creates a Uniform, to be recognized in shaders to be loaded. This
/// means that a uniform of a given name should always have the same type
/// and array size across the board. Note that the maximum array size
/// is 255.
///@note If a uniform with the given name (and matching type and size)
/// already exists, its refCount is incremented instead.
UniformHandle CreateUniform(char const* name, UniformType type, uint8_t arraySize = 1);

///@brief Decrements the refCount of a uniform, and if it reaches 0,
/// deallocates it.
///@note Programs' usage of a uniform also increments its refCount.
void Destroy(UniformHandle h);

///@brief Compiles preprocessed shader. Source isn't kept around.
///@return Handle, if the shader compilation was successful. INVALID_HANDLE
/// if not.
ShaderHandle CreateShader(ShaderType t, Buffer const& buffer);
void Destroy(ShaderHandle h);

///@brief Creates and links a shader program from the given vertex
/// and fragment shaders.
///@return Handle, if linking was successful. INVALID_HANDLE if not.
ProgramHandle CreateProgram(ShaderHandle hVertex, ShaderHandle hFragment);

///@brief Destroys the given program and decrements the ref count of the shaders
/// and uniforms it uses.
void Destroy(ProgramHandle h);

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

///@brief Sets render state. See the F_STATE_... flags.
void SetState(uint32_t flags = F_STATE_NONE);

///@brief Sets instance data to be used for the subsequent Draw calls.
void SetInstanceData(InstanceDataBufferHandle h, uint16_t offset, uint16_t count);

///@brief Sets a shader program to be used for the subsequent Draw calls.
void SetProgram(ProgramHandle h);

///@brief Sets a render target to render to. To set the default render target,
/// pass invalid handle.
void SetFrameBuffer(FrameBufferHandle h);

///@brief Draws the contents of the given vertex buffer; @a count vertices
/// starting from @a offset.
void Draw(VertexBufferHandle vbh, PrimType primitiveType, uint32_t offset, uint32_t count);

///@brief Draws the contents of the given vertex buffer, using @a count indices
/// starting from @a offset.
void Draw(VertexBufferHandle vbh, IndexBufferHandle ibh, PrimType primitiveType, uint32_t offset, uint32_t count);

void Clear(uint32_t flags, Color color = Color(0xff000000), float depth = 1.0f,
  uint8_t stencil = 0x00);

void Flush();
void Present(bool resetState = true);

///@brief Registers a function to be called upon Flush().
void RegisterFlushCallback(Callback fn, void* userData);

///@brief Registers a function to be called upon Exit().
void RegisterExitCallback(Callback fn, void* userData);

///@brief shuts down the renderer.
void Exit();

} // GFX
} // XR

#endif // XR_GFX_HPP

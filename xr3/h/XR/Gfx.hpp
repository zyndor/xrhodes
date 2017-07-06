//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_GFX_HPP
#define XR_GFX_HPP

#include <XR/Color.hpp>
#include <XR/Matrix.hpp>
#include <XR/Quaternion.hpp>
#include <XR/Mat4x4Helper.hpp>
#include <XR/memory.hpp>
#include <XR/utils.hpp>
#include <XR/Hash.hpp>
#include <XR/PrimType.hpp>
#include <XR/debug.hpp>

namespace XR
{
namespace Gfx
{

//=============================================================================
struct Buffer
{
  uint8_t const* data;
  size_t size;
};

//=============================================================================
enum Flags
{
  F_BUFFER_NONE = 0,
  F_BUFFER_INDIRECT_DRAW = XR_MASK_ID(0),

  F_TEXTURE_NONE = 0,
  F_TEXTURE_WRAP = XR_MASK_ID(0), // clamp is default
  F_TEXTURE_FILTER_POINT = XR_MASK_ID(1), // linear filtering is default
  F_TEXTURE_SRGB = XR_MASK_ID(2),
  F_TEXTURE_MIPMAPS = XR_MASK_ID(6),  // no mipmaps by default
  F_TEXTURE_CUBE = XR_MASK_ID(7),

  F_STATE_NONE = 0,
  F_STATE_DEPTH_TEST = XR_MASK_ID(0), // off by default
  F_STATE_DEPTH_WRITE = XR_MASK_ID(1), // off by default
  F_STATE_ALPHA_BLEND = XR_MASK_ID(2), // off by default
  F_STATE_CULL_BACK = XR_MASK_ID(3),  // no culling by default
  F_STATE_CULL_FRONT = XR_MASK_ID(4), // only points and lines drawn when F_STATE_CULL_BACK | F_STATE_CULL_FRONT set
  F_STATE_WIREFRAME = XR_MASK_ID(5),

  F_CLEAR_NONE = 0,
  F_CLEAR_COLOR = XR_MASK_ID(0),
  F_CLEAR_DEPTH = XR_MASK_ID(1),
  F_CLEAR_STENCIL = XR_MASK_ID(2),
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
    XR_ASSERT(VertexFormat, numComponents > 0 && numComponents < 4);
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
    return XR::Hash::Data(m_attributes, sizeof(m_attributes) + sizeof(m_offset));
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
namespace
{
struct HandleCoreCore {
  enum : uint16_t { INVALID_ID = 0xffff };

  uint16_t id = INVALID_ID;
  bool IsValid() const
  {
    return id != INVALID_ID;
  }
};

template <typename T>
struct HandleCore : HandleCoreCore
{
  bool operator==(T const& rhs) const { return id == rhs.id; }
  bool operator<(T const& rhs) const { return id < rhs.id; }
};
}

#define GFX_HANDLE_DECL(name) struct name: HandleCore<name> {}
GFX_HANDLE_DECL(VertexFormatHandle);
GFX_HANDLE_DECL(VertexBufferHandle);
GFX_HANDLE_DECL(TextureHandle);
//GFX_HANDLE_DECL(RenderTargetHandle);  // TODO: render targets
GFX_HANDLE_DECL(ShaderHandle);
GFX_HANDLE_DECL(ProgramHandle);
GFX_HANDLE_DECL(UniformHandle);
#undef GFX_HANDLE_DECL

//=============================================================================
///@brief Initialises Gfx with the given window handle, which must match the
/// implementation of Device.
void Init(void* window, Allocator* alloc = nullptr);

///@brief shuts down the renderer.
void Exit();

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

///@brief Uploads texture. Texel data isn't kept around by Gfx.
TextureHandle CreateTexture(TextureFormat hFormat, uint32_t width,
  uint32_t height, uint32_t depth, uint32_t flags, Buffer const* buffer,
  size_t numBuffers = 1);

TextureInfo const& GetTextureInfo(TextureHandle h);

// TODO: texture read.

///@brief Decrements refcount of a texture, and if it has reached 0 zero,
/// deletes it.
void Destroy(TextureHandle h);

// Uniforms
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

void Destroy(ProgramHandle h);

// State and drawing
///@brief Sets a value for the given uniform.
void SetUniform(UniformHandle h, uint8_t num, void const* data);

///@brief Binds the given texture for a texture stage.
void SetTexture(TextureHandle h, uint32_t stage = 0);

///@brief Sets render state. See the F_STATE_... flags.
void SetState(uint32_t flags = F_STATE_NONE);

///@brief Sets a shader program to be used for the subsequent Draw calls.
void SetProgram(ProgramHandle h);

///@brief Draws the contents of the given vertex buffer.
void Draw(VertexBufferHandle vbh, PrimType primitiveType, uint32_t offset, uint32_t count);

// TODO: indexed drawing

void Clear(uint32_t flags, Color color = Color(0xff000000), float depth = 1.0f,
  uint8_t stencil = 0x00);

void Flush();
void Present(bool resetState = true);

} // GFX
} // XR

#endif // XR_GFX_HPP

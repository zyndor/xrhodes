//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Gfxp.hpp"
#include "xrgl.hpp"
#include <XR/Matrix.hpp>
#include <XR/Device.hpp>
#include <XR/SVector2.hpp>
#include <XR/Pool.hpp>
#include <XR/Hash.hpp>
#include <SDL.h>
#include <GL/glew.h>

#include <set>
#include <unordered_map>
#include <functional>

namespace XR
{
namespace Gfx
{

//=============================================================================
char const *const  kAttributeName[] =
{
	"aPosition",
	"aNormal",
	"aTangent",
	"aBitangent",
	"aColor0",
	"aColor1",
	"aUV0",
	"aUV1",
	"aUV2",
	"aUV3",
	"aUV4",
	"aUV5",
	"aUV6",
	"aUV7",
};
static_assert(XR_ARRAY_SIZE(kAttributeName) == size_t(Attribute::kCount),
  "Count of attribute types / names must match.");

//=============================================================================
const GLenum kAttachmentTypes[] =
{
  GL_COLOR_ATTACHMENT0,
  GL_DEPTH_ATTACHMENT,
  GL_STENCIL_ATTACHMENT,
  GL_DEPTH_STENCIL_ATTACHMENT
};

//=============================================================================
const GLenum kShaderTypes[] = {
  GL_VERTEX_SHADER,
  GL_FRAGMENT_SHADER,
};

//=============================================================================
const GLenum kPrimitiveTypes[] =
{
  GL_LINES,
  GL_LINE_STRIP,
  GL_TRIANGLES,
  GL_TRIANGLE_STRIP,
  GL_QUADS,
  GL_QUAD_STRIP
};

//=============================================================================
struct ExtensionGL
{
  enum Name
  {
    ARB_vertex_array_object,
    ARB_program_interface_query,
    ARB_shader_storage_buffer_object,

    OES_vertex_array_object,

    kCount
  };

  char const* const name;
  bool supported;
};

ExtensionGL s_extensions[] =
{
  { "ARB_vertex_array_object",            false },
  { "ARB_program_interface_query",        false },
  { "ARB_shader_storage_buffer_object",   false },
  { "OES_vertex_array_object",            false },
};
static_assert(XR_ARRAY_SIZE(s_extensions) == size_t(ExtensionGL::Name::kCount),
  "Count of extension names / definitions must match.");

//=============================================================================
struct VertexFormatRef
{
  VertexFormat format;
  uint16_t refCount = 0;
};

//=============================================================================
struct ResourceGL
{
  GLuint name = -1;
};

//=============================================================================
struct VertexBufferObject : ResourceGL
{
  VertexFormatHandle hFormat;
  uint32_t flags = F_BUFFER_NONE;
  GLenum  target;

  void Bind() const
  {
    XR_GL_CALL(glBindBuffer(target, name));
  }
};

//=============================================================================
struct IndexBufferObject : ResourceGL
{
  uint8_t indexSize = 0;
  uint32_t flags = F_BUFFER_NONE;
  GLenum type;

  void Bind() const
  {
    XR_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, name));
  }
};

//=============================================================================
struct Texture: ResourceGL
{
  struct Format
  {
    GLenum internal;
    GLenum srgbInternal;
    GLenum format;
    GLenum type;
    // 2 bits for attachment type, then 6 bits for bit size each component.
    uint16_t colorDepthStencilBits;
    bool compressed;
  };

  TextureInfo info;
  GLenum target = GL_NONE;
  uint16_t refCount = 0;

  void Bind()
  {
    XR_GL_CALL(glBindTexture(target, name));
  }
};

#define PACK_TEX_COMP(size, component) ((size) & 0x3f) << (2 + ((component) * 6))

const Texture::Format kTextureFormats[] =
{
  { GL_R8, GL_ZERO, GL_RED, GL_UNSIGNED_BYTE,
    uint8_t(AttachmentType::Color) | PACK_TEX_COMP(8, 0), false },
  { GL_RGB565, GL_ZERO, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
    uint8_t(AttachmentType::Color) | PACK_TEX_COMP(16, 0), false },
  { GL_RGB8, GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE,
    uint8_t(AttachmentType::Color) | PACK_TEX_COMP(24, 0), false },
  { GL_RGBA8, GL_SRGB8_ALPHA8, GL_RGBA8, GL_UNSIGNED_BYTE,
    uint8_t(AttachmentType::Color) | PACK_TEX_COMP(32, 0), false },
  { GL_RGBA8, GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_BYTE,
    uint8_t(AttachmentType::Color) | PACK_TEX_COMP(32, 0), false },

  // compressed
  // depth/stencil
  { GL_DEPTH_COMPONENT32, GL_ZERO, GL_UNSIGNED_INT,
    uint8_t(AttachmentType::Depth) | PACK_TEX_COMP(32, 0), false },
  { GL_DEPTH24_STENCIL8, GL_ZERO, GL_UNSIGNED_INT_24_8,
    uint8_t(AttachmentType::DepthStencil) | PACK_TEX_COMP(24, 0) | PACK_TEX_COMP(8, 1), false },
  { GL_STENCIL_INDEX, GL_ZERO, GL_UNSIGNED_BYTE,
    uint8_t(AttachmentType::Stencil) | PACK_TEX_COMP(8, 0), false },
};
static_assert(XR_ARRAY_SIZE(kTextureFormats) == size_t(TextureFormat::kCount),
  "Count of texture formats / definition must match.");

#undef PACK_TEX_COMP

//=============================================================================
struct FrameBuffer : ResourceGL
{
  TextureHandle hTextures[8];
  uint8_t numTextures;
  bool    ownTextures;

  void Bind()
  {
    XR_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, name));
  }
};

//=============================================================================
struct Shader : ResourceGL
{
  ShaderType type;
  uint16_t refCount = 0;
};

//=============================================================================
struct Program: ResourceGL
{
  ShaderHandle  hVertex;
  ShaderHandle  hFragment;
  bool          linked = false;
  int           numActiveAttribs = 0;
  uint8_t       activeAttribs[XR_ARRAY_SIZE(kAttributeName)]; // order to XR attrib id
  GLint         attribLoc[XR_ARRAY_SIZE(kAttributeName)]; // XR attrib id to GLSL location
  uint8_t       unboundAttribs[XR_ARRAY_SIZE(kAttributeName)]; // bound from VertexFormat
  ConstBuffer*  uniforms = nullptr;
  // TODO: latest update timestamp

  void UpdateUniforms(size_t numUniforms, Uniform const* uniformData)
  {
    if (uniforms)
    {
      uniforms->Reset();

      UniformType type;
      uint8_t arraySize;
      uint16_t ignoreLoc;
      UniformHandle hUniform;
      GLint loc;
      while (!uniforms->AtEnd())
      {
        uniforms->ReadHandle(type, arraySize, ignoreLoc, hUniform);
        uniforms->Read(loc);

        XR_ASSERT(Gfx, numUniforms > hUniform.id);
        void const* value = uniformData[hUniform.id].data;
        switch (type)
        {
        case UniformType::Int1:
          XR_GL_CALL(glUniform1iv(loc, arraySize, reinterpret_cast<GLint const*>(value)));
          break;

        case UniformType::Vec4:
          XR_GL_CALL(glUniform4fv(loc, arraySize, reinterpret_cast<GLfloat const*>(value)));
          break;

        case UniformType::Mat3:
          XR_GL_CALL(glUniformMatrix3fv(loc, arraySize, GL_FALSE, reinterpret_cast<GLfloat const*>(value)));
          break;

        case UniformType::Mat4:
          XR_GL_CALL(glUniformMatrix4fv(loc, arraySize, GL_FALSE, reinterpret_cast<GLfloat const*>(value)));
          break;
        }
      }
    }
  }

  void ReleaseUniforms(size_t numUniforms, Uniform* uniformData)
  {
    uniforms->Reset();

    UniformType ignoreType;
    uint8_t ignoreSize;
    uint16_t ignoreLoc;
    UniformHandle hUniform;
    while (!uniforms->AtEnd())
    {
      uniforms->ReadHandle(ignoreType, ignoreSize, ignoreLoc, hUniform);
      XR_ASSERT(Gfx, hUniform.id < numUniforms);
      --(uniformData[hUniform.id].refCount);
    }

    uniforms->Finalize();
  }

  void ResetAttribBindings()
  {
    static_assert(sizeof(activeAttribs) == sizeof(unboundAttribs), "Size of active / unbound attribs array must match.");
    std::memcpy(unboundAttribs, activeAttribs, sizeof(unboundAttribs));
  }

  void BindAttributes(VertexFormat const& format, uint32_t baseVertex)
  {
    const uint32_t stride = format.GetStride();
    uint8_t numComponents;
    bool normalized;
    uint16_t offset;
    for (int i = 0; i < numActiveAttribs; ++i)
    {
      uint8_t attrib = activeAttribs[i];
      GLint loc = attribLoc[attrib];

      if (loc != -1 && format.Get(Attribute(attrib), numComponents, normalized, offset))
      {
        XR_GL_CALL(glEnableVertexAttribArray(loc));
        XR_GL_CALL(glVertexAttribDivisor(loc, 0));

        uint32_t attribBase = baseVertex * stride + offset;;
        XR_GL_CALL(glVertexAttribPointer(loc, numComponents, GL_FLOAT, normalized, format.GetStride(),
          reinterpret_cast<void*>(uintptr_t(attribBase))));

        unboundAttribs[i] = uint8_t(Attribute::kCount);
      }
    }
  }

  void UnbindAttributes() const
  {
    for (int i = 0; i < numActiveAttribs; ++i)
    {
      if (unboundAttribs[i] != uint8_t(Attribute::kCount))
      {
        uint8_t attrib = activeAttribs[i];
        GLint loc = attribLoc[attrib];

        XR_GL_CALL(glDisableVertexAttribArray(loc));
      }
    }
  }
};

//=============================================================================
char const* GetGLSLTypeName(GLenum type)
{
#define GLSL_TYPE(t) case t: return #t

  switch (type)
  {
    GLSL_TYPE(GL_INT);
    GLSL_TYPE(GL_INT_VEC2);
    GLSL_TYPE(GL_INT_VEC3);
    GLSL_TYPE(GL_INT_VEC4);
    GLSL_TYPE(GL_UNSIGNED_INT);
    GLSL_TYPE(GL_UNSIGNED_INT_VEC2);
    GLSL_TYPE(GL_UNSIGNED_INT_VEC3);
    GLSL_TYPE(GL_UNSIGNED_INT_VEC4);
    GLSL_TYPE(GL_FLOAT);
    GLSL_TYPE(GL_FLOAT_VEC2);
    GLSL_TYPE(GL_FLOAT_VEC3);
    GLSL_TYPE(GL_FLOAT_VEC4);
    GLSL_TYPE(GL_FLOAT_MAT2);
    GLSL_TYPE(GL_FLOAT_MAT3);
    GLSL_TYPE(GL_FLOAT_MAT4);

    GLSL_TYPE(GL_SAMPLER_2D);
    GLSL_TYPE(GL_SAMPLER_2D_ARRAY);
    GLSL_TYPE(GL_SAMPLER_2D_MULTISAMPLE);

    GLSL_TYPE(GL_INT_SAMPLER_2D);
    GLSL_TYPE(GL_INT_SAMPLER_2D_ARRAY);
    GLSL_TYPE(GL_INT_SAMPLER_2D_MULTISAMPLE);

    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_2D);
    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY);
    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE);

    GLSL_TYPE(GL_SAMPLER_2D_SHADOW);
    GLSL_TYPE(GL_SAMPLER_2D_ARRAY_SHADOW);

    GLSL_TYPE(GL_SAMPLER_3D);
    GLSL_TYPE(GL_INT_SAMPLER_3D);
    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_3D);

    GLSL_TYPE(GL_SAMPLER_CUBE);
    GLSL_TYPE(GL_INT_SAMPLER_CUBE);
    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_CUBE);

    GLSL_TYPE(GL_IMAGE_1D);
    GLSL_TYPE(GL_INT_IMAGE_1D);
    GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_1D);

    GLSL_TYPE(GL_IMAGE_2D);
    GLSL_TYPE(GL_IMAGE_2D_ARRAY);
    GLSL_TYPE(GL_INT_IMAGE_2D);
    GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_2D);

    GLSL_TYPE(GL_IMAGE_3D);
    GLSL_TYPE(GL_INT_IMAGE_3D);
    GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_3D);

    GLSL_TYPE(GL_IMAGE_CUBE);
    GLSL_TYPE(GL_INT_IMAGE_CUBE);
    GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_CUBE);

  default:
    return "<unknown>";
  }
#undef GLSL_TYPE
}
//=============================================================================
UniformType ImportGLSLType(GLenum type)
{
  switch (type)
  {
  case GL_INT:
  case GL_UNSIGNED_INT:

  case GL_SAMPLER_2D:
  case GL_SAMPLER_2D_ARRAY:
  case GL_SAMPLER_2D_MULTISAMPLE:

  case GL_INT_SAMPLER_2D:
  case GL_INT_SAMPLER_2D_ARRAY:
  case GL_INT_SAMPLER_2D_MULTISAMPLE:

  case GL_UNSIGNED_INT_SAMPLER_2D:
  case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
  case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:

  case GL_SAMPLER_2D_SHADOW:
  case GL_SAMPLER_2D_ARRAY_SHADOW:

  case GL_SAMPLER_3D:
  case GL_INT_SAMPLER_3D:
  case GL_UNSIGNED_INT_SAMPLER_3D:

  case GL_SAMPLER_CUBE:
  case GL_INT_SAMPLER_CUBE:
  case GL_UNSIGNED_INT_SAMPLER_CUBE:

  case GL_IMAGE_1D:
  case GL_INT_IMAGE_1D:
  case GL_UNSIGNED_INT_IMAGE_1D:

  case GL_IMAGE_2D:
  case GL_INT_IMAGE_2D:
  case GL_UNSIGNED_INT_IMAGE_2D:

  case GL_IMAGE_3D:
  case GL_INT_IMAGE_3D:
  case GL_UNSIGNED_INT_IMAGE_3D:

  case GL_IMAGE_CUBE:
  case GL_INT_IMAGE_CUBE:
  case GL_UNSIGNED_INT_IMAGE_CUBE:
    return UniformType::Int1;

  case GL_FLOAT:
  case GL_FLOAT_VEC2:
  case GL_FLOAT_VEC3:
  case GL_FLOAT_VEC4:
    return UniformType::Vec4;

  case GL_FLOAT_MAT3:
    return UniformType::Mat4;

  case GL_FLOAT_MAT4:
    return UniformType::Mat4;

  default:
    return UniformType::kCount;
  }
}

//=============================================================================
struct Context
{
  static void InitExtensions(int minorMajorVersion)
  {
    auto bufferCmp = [](Buffer const& lhs, Buffer const& rhs) {
      return strncmp((char const*)lhs.data, (char const*)rhs.data,
        std::max(lhs.size, rhs.size)) < 0;
    };
    std::set<Buffer, decltype(bufferCmp)> extensions(bufferCmp);

    // get extensions
    char const* ext = (char const*)glGetString(GL_EXTENSIONS);
    XR_GL_IGNORE_ERROR;
    if(ext)
    {
      do {
        if (strncmp(ext, "GL_", 3) == 0)
        {
          ext += 3;
        }
        size_t span = strcspn(ext, " ");
        extensions.insert({ (uint8_t const*)ext, span });
        
        ext += span;
        if (*ext == ' ')
        {
          ++ext;
        }
        else
        {
          break;
        }
      } while (ext);
    }
    else if (glGetStringi)
    {
      GLint numExtensions = 0;
      glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
      XR_GL_IGNORE_ERROR;

      for (GLint index = 0; index < numExtensions; ++index)
      {
        char const* ext = (char const*)glGetStringi(GL_EXTENSIONS, index);
        if (strncmp(ext, "GL_", 3) == 0)
        {
          ext += 3;
        }
        extensions.insert({ (uint8_t const*)ext, strlen(ext) });
      }
    }

    // check support now
    for (auto& i : s_extensions)
    {
      i.supported = extensions.end() !=
        extensions.find(Buffer{ (uint8_t const*)i.name, strlen(i.name) });
    }
  }

  Context(SDL_Window* window)
  {
    XR_ASSERTMSG(Renderer, window, ("Window must not be null!"));
    m_window = window; // static_cast<SDL_Window*>(mainWindow);

    // Window must be created at this point. In SDL, we do it in Device.
    // Create SDL renderer.
    uint32_t flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
    if (Device::GetConfigInt("Display", "vsync", false))
    {
      flags |= SDL_RENDERER_PRESENTVSYNC;
    }

    m_ownRenderer = SDL_CreateRenderer(window, -1, flags);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    // Create GL context.
    int openGlVersionMajor = Device::GetConfigInt("GL", "versionMajor", 3);
    int openGlVersionMinor = Device::GetConfigInt("GL", "versionMinor", 1);
    int openGLUseCompatibilityProfile = Device::GetConfigInt("GL", "useCompatibility", 0);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, openGlVersionMajor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, openGlVersionMinor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, openGLUseCompatibilityProfile > 0 ?
      SDL_GL_CONTEXT_PROFILE_COMPATIBILITY : SDL_GL_CONTEXT_PROFILE_CORE);

    int width, height;
    SDL_GL_GetDrawableSize(window, &width, &height);

    SDL_RenderSetLogicalSize(m_ownRenderer, width, height);

    m_ownContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, m_ownContext);

    // Get display and screen sizes.
    SDL_RendererInfo  info;
    SDL_GetRendererInfo(m_ownRenderer, &info);

    XR_ASSERT(Renderer, info.flags & SDL_RENDERER_ACCELERATED);
    XR_ASSERT(Renderer, info.flags & SDL_RENDERER_TARGETTEXTURE);

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);

    m_physicalSize = SVector2(dm.w, dm.h);
    m_logicalSize = SVector2(width, height);

    // Init GLEW.
    glewExperimental = GL_TRUE;
    XR_GL_CALL(glewInit());

    // initialise extensions
    InitExtensions(openGlVersionMajor * 10 + openGlVersionMinor);

    SetViewport({ 0, 0, uint16_t(m_logicalSize.x), uint16_t(m_logicalSize.y) });

    XR_GL_CALL(glDepthFunc(GL_LEQUAL)); // TODO: give it to state

    if (s_extensions[ExtensionGL::OES_vertex_array_object].supported ||
      s_extensions[ExtensionGL::ARB_vertex_array_object].supported)
    {
      XR_GL_CALL(glGenVertexArrays(1, &m_vao));
      XR_GL_CALL(glBindVertexArray(m_vao));
    }

    // initialise frame pool
    int poolSize(Device::GetConfigInt("GFX", "framePoolSize", 128000));
    m_framePool.SetBuffer(poolSize, true, 0);
  }

  ~Context()
  {
    if (m_vao != 0)
    {
      XR_GL_CALL(glDeleteVertexArrays(1, &m_vao));
    }

    // TODO: report resource leaks.

    SDL_GL_DeleteContext(m_ownContext);
    SDL_DestroyRenderer(m_ownRenderer);
  }

  uint16_t GetWidth() const
  {
    return m_logicalSize.x;
  }

  uint16_t GetHeight() const
  {
    return m_logicalSize.y;
  }

  VertexFormatHandle RegisterVertexFormat(VertexFormat const& format)
  {
    uint32_t const hash = format.CalculateHash();
    auto iFind = m_vertexFormatHandles.find(hash);
    VertexFormatHandle hFormat;
    if (iFind != m_vertexFormatHandles.end())
    {
      hFormat = iFind->second;
      ++(m_vertexFormats[hFormat.id].refCount);
    }
    else
    {
      hFormat.id = uint16_t(m_vertexFormats.server.Acquire());
      VertexFormatRef& vf = m_vertexFormats[hFormat.id];
      vf.format = format;
      vf.refCount = 1;
    }
    return hFormat;
  }

  void Destroy(VertexFormatHandle h)
  {
    VertexFormatRef vfr = m_vertexFormats[h.id];
    XR_ASSERT(Gfx, vfr.refCount > 0);
    --vfr.refCount;
    if (vfr.refCount == 0)
    {
      uint32_t hash = vfr.format.CalculateHash();
      m_vertexFormatHandles.erase(hash);
    }
  }

  VertexBufferHandle CreateVertexBuffer(VertexFormatHandle hFormat, Buffer const& buffer, uint32_t flags)
  {
    VertexBufferHandle h;
    h.id = uint16_t(m_vbos.server.Acquire());

    VertexBufferObject& vbo = m_vbos[h.id];
    XR_GL_CALL(glGenBuffers(1, &vbo.name));

    vbo.target = IsFullMask(flags, F_BUFFER_INDIRECT_DRAW) ?
      GL_DRAW_INDIRECT_BUFFER : GL_ARRAY_BUFFER;  // TODO: indirect draw support
    vbo.Bind();

    XR_GL_CALL(glBufferData(vbo.target, buffer.size, buffer.data,
      buffer.data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW));
    XR_GL_CALL(glBindBuffer(vbo.target, 0));

    vbo.hFormat = hFormat;
    vbo.flags = flags;

    return h;
  }

  void Destroy(VertexBufferHandle h)
  {
    VertexBufferObject& vbo = m_vbos[h.id];
    XR_GL_CALL(glBindBuffer(vbo.target, 0));
    XR_GL_CALL(glDeleteBuffers(1, &vbo.name));

    std::memset(&vbo, 0x00, sizeof(vbo));
    // TODO: invalidate vaos.
  }

  IndexBufferHandle CreateIndexBuffer(Buffer const& buffer, uint32_t flags)
  {
    IndexBufferHandle h;
    h.id = m_ibos.server.Acquire();

    IndexBufferObject& ibo = m_ibos[h.id];
    XR_GL_CALL(glGenBuffers(1, &ibo.name));
    ibo.Bind();
    XR_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.size, buffer.data,
      buffer.data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW));

    ibo.indexSize = IsFullMask(flags, F_BUFFER_INDEX_32BITS) ? sizeof(uint32_t) : sizeof(uint16_t);
    ibo.flags = flags;

    return h;
  }

  void Destroy(IndexBufferHandle h)
  {
    IndexBufferObject& ibo = m_ibos[h.id];
    XR_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    XR_GL_CALL(glDeleteBuffers(1, &ibo.name));

    std::memset(&ibo, 0x00, sizeof(ibo));
    // TODO: invalidate vaos.
  }

  TextureHandle CreateTexture(TextureFormat format, uint32_t width,
    uint32_t height, uint32_t depth, uint32_t flags, Buffer const* buffer,
    size_t numBuffers)
  {
    XR_ASSERT(Gfx, width > 0);
    XR_ASSERT(Gfx, height > 0);
    XR_ASSERT(Gfx, numBuffers > 0);
    TextureHandle h;
    h.id = uint16_t(m_textures.server.Acquire());

    Texture& t = m_textures[h.id];
    XR_GL_CALL(glGenTextures(1, &t.name));
    t.info.format = format;
    t.info.width = width;
    t.info.height = height;
    t.info.depth = depth;
    t.info.flags = flags;
    
    // determine target
    t.target = IsFullMask(flags, F_TEXTURE_CUBE) ? GL_TEXTURE_CUBE_MAP : 
      (depth > 0 ? GL_TEXTURE_3D : GL_TEXTURE_2D);
    XR_ASSERT(Gfx, t.target != GL_TEXTURE_CUBE_MAP || (numBuffers % 6) == 0);

    // bind it
    t.Bind();

    GLenum  wrapMode(IsFullMask(flags, F_TEXTURE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_WRAP_S, wrapMode));
    XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_WRAP_T, wrapMode));
    if (t.target == GL_TEXTURE_3D)
    {
      XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_WRAP_R, wrapMode));
    }

    GLenum  filterMode(IsFullMask(flags, F_TEXTURE_FILTER_POINT) ? GL_NEAREST : GL_LINEAR);
    XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_MIN_FILTER, filterMode));
    XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_MAG_FILTER, filterMode));

    auto& formatDesc = kTextureFormats[int(format)];
    GLenum fmtInternal = (IsFullMask(flags, F_TEXTURE_SRGB) && formatDesc.srgbInternal != GL_ZERO) ?
      formatDesc.srgbInternal : formatDesc.internal;

    GLenum target = t.target != GL_TEXTURE_CUBE_MAP ? t.target : GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    const int sides = t.target != GL_TEXTURE_CUBE_MAP ? 1 : 6;
    const int mipMapLevels(IsFullMask(flags, F_TEXTURE_MIPMAPS) ?
      int(std::log2(std::min(width, height))) : 1);
    for (int i = 0; i < mipMapLevels; ++i)
    {
      for (int j = 0; j < sides; ++j)
      {
        if (formatDesc.compressed)
        {
          switch (t.target)
          {
          case GL_TEXTURE_3D:
            XR_GL_CALL(glCompressedTexImage3D(target, i, fmtInternal, width, height, depth,
              0, buffer->size, buffer->data));
            break;

          case GL_TEXTURE_2D:
          case GL_TEXTURE_CUBE_MAP:
            XR_GL_CALL(glCompressedTexImage2D(target + j, i, fmtInternal,
              width, height, 0, buffer[j].size, buffer[j].data));
            break;
          }
        }
        else
        {
          switch (t.target)
          {
          case GL_TEXTURE_3D:
            XR_GL_CALL(glTexImage3D(t.target, i, fmtInternal, width, height, depth,
              0, formatDesc.format, formatDesc.type, buffer->data));
            break;

          case GL_TEXTURE_2D:
          case GL_TEXTURE_CUBE_MAP:
            XR_GL_CALL(glTexImage2D(target + j, i, fmtInternal,
              width, height, 0, formatDesc.format, formatDesc.type, buffer[j].data));
            break;
          }
        }
      }

      width /= 2;
      height /= 2;
    }

    return h;
  }

  TextureInfo const& GetTextureInfo(TextureHandle h)
  {
    XR_ASSERT(Gfx, h.id < m_textures.server.GetNumActive());
    return m_textures[h.id].info;
  }

  void Destroy(TextureHandle h)
  {
    Texture& texture = m_textures[h.id];
    XR_ASSERT(Gfx, texture.refCount > 0);
    --texture.refCount;
    if (texture.refCount == 0)
    {
      XR_GL_CALL(glDeleteTextures(1, &texture.name));
      texture.name = 0;

      m_textures.server.Release(h.id);
    }
  }

  FrameBufferHandle CreateFrameBuffer(uint8_t textureCount, TextureHandle const* hTextures, bool ownTextures)
  {
    std::vector<FrameBufferAttachment>  attachments(textureCount);
    std::generate(attachments.begin(), attachments.end(), [&hTextures]() {
      FrameBufferAttachment a = { *hTextures, 0, 0 };
      ++hTextures;
      return a;
    });
    return CreateFrameBuffer(textureCount, attachments.data(), ownTextures);
  }

  FrameBufferHandle CreateFrameBuffer(uint8_t textureCount, FrameBufferAttachment const* attachments, bool ownTextures)
  {
    XR_ASSERT(Gfx, textureCount < XR_ARRAY_SIZE(FrameBuffer::hTextures));
    FrameBuffer rt;

    XR_GL_CALL(glGenFramebuffers(1, &rt.name));
    rt.Bind();

    rt.numTextures = textureCount;
    uint8_t colorAttachments[XR_ARRAY_SIZE(FrameBuffer::hTextures)];
    int numColorAttachments = 0;
    for (uint8_t i = 0; i < textureCount; ++i)
    {
      FrameBufferAttachment const& att = attachments[i];
      rt.hTextures[i] = att.hTexture;

      Texture& texture = m_textures[att.hTexture.id];
      XR_ASSERT(Gfx, texture.info.format != TextureFormat::kCount);
      uint16_t bits = kTextureFormats[uint8_t(texture.info.format)].colorDepthStencilBits;
      GLenum attachmentType = kAttachmentTypes[bits & 0x3];
      if (attachmentType == GL_COLOR_ATTACHMENT0)
      {
        attachmentType += numColorAttachments;
        colorAttachments[numColorAttachments] = attachmentType;
        ++numColorAttachments;
      }

      GLenum target = texture.target;
      if (GL_TEXTURE_CUBE_MAP == target)
      {
        target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + att.side;
      }

      XR_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, target,
        texture.name, att.mipLevel));

      if (!ownTextures)
      {
        ++texture.refCount;
      }
    }

    FrameBufferHandle h;
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (GL_FRAMEBUFFER_COMPLETE == status)
    {
      h.id = uint16_t(m_renderTargets.server.Acquire());

      m_renderTargets[h.id] = rt;
    }
    else
    {
      XR_TRACE(Gfx, ("Failed to create framebuffer, status: 0x%x", status));

      if (!ownTextures)
      {
        for (uint8_t i = 0; i < textureCount; ++i)
        {
          Destroy(attachments[i].hTexture);
        }
      }
    }

    return h;
  }

  FrameBufferHandle CreateFrameBuffer(TextureFormat format, uint32_t width, uint32_t height,
    uint32_t flags)
  {
    Buffer buffer = { 0, 0 };
    TextureHandle h = CreateTexture(format, width, height, 0, flags, &buffer, 1);
    return CreateFrameBuffer(1, &h, true);
  }

  void Destroy(FrameBufferHandle h)
  {
    FrameBuffer& rt = m_renderTargets[h.id];
    rt.Bind();

    for (uint8_t i = 0; i < rt.numTextures; ++i)
    {
      Destroy(rt.hTextures[i]);
    }
    std::memset(&rt, 0x00, sizeof(FrameBuffer));

    XR_GL_CALL(glDeleteFramebuffers(1, &rt.name));
    m_renderTargets.server.Release(h.id);
  }

  UniformHandle CreateUniform(char const* name, UniformType type, uint8_t arraySize)
  {
    // TODO: reserved uniforms
    uint32_t const hash = XR::Hash::String(name);
    auto iFind = m_uniformHandles.find(hash);
    UniformHandle h;
    bool existing = iFind != m_uniformHandles.end();
    if (existing)
    {
      // Get existing uniform
      h = iFind->second;
    }
    else
    {
      // Create uniform anew.
      h.id = uint16_t(m_uniforms.server.Acquire());
    }

    Uniform& u = m_uniforms[h.id];
    XR_ASSERTMSG(Gfx, !existing || type == u.type, ("Uniform already registered as %d; mismatched type: %d", u.type, type));
    XR_ASSERTMSG(Gfx, !existing || arraySize == u.arraySize, ("Uniform already registered as %d; mismatched size: %d", u.arraySize, arraySize));
    if (!existing)
    {
      u.refCount = 1;
      u.name = name;
      u.type = type;
      u.arraySize = std::max(arraySize, uint8_t(1));

      size_t requiredBytes = u.arraySize * kUniformTypeSize[uint8_t(u.type)];
      u.data = g_allocator->Allocate(requiredBytes);
      std::memset(u.data, 0x00, requiredBytes);

      m_uniformHandles[hash] = h;
    }
    else
    {
      ++u.refCount;
    }

    return h;
  }

  void Destroy(UniformHandle h)
  {
    Uniform& u = m_uniforms[h.id];
    XR_ASSERT(Gfx, u.refCount > 0);
    --u.refCount;
    if (u.refCount == 0)
    {
      g_allocator->Deallocate(m_uniforms[h.id].data);
      std::memset(&u, 0x00, sizeof(Uniform));

      m_uniforms.server.Release(h.id);
    }
  }

  ShaderHandle CreateShader(ShaderType type, Buffer const& buffer)
  {
    Shader shader;

    GLenum typegl = kShaderTypes[size_t(type)];
    XR_GL_CALL(shader.name = glCreateShader(typegl));

    XR_GL_CALL(glShaderSource(shader.name, 1, reinterpret_cast<GLchar const* const*>(&buffer.data), nullptr));
    XR_GL_CALL(glCompileShader(shader.name));

    GLint params = -1;
    XR_GL_CALL(glGetShaderiv(shader.name, GL_COMPILE_STATUS, &params));
    bool success = params == GL_TRUE;
    ShaderHandle h;
    if (success)
    {
      shader.refCount = 1;

      h.id = uint16_t(m_shaders.server.Acquire());
      m_shaders[h.id] = shader;
    }
    else
    {
      char log[1024];
      XR_GL_CALL(glGetShaderInfoLog(shader.name, sizeof(log), 0, log));
      XR_TRACE(Gfx, ("Failed to compile shader: %s", log));

      XR_GL_CALL(glDeleteShader(shader.name));
    }
    return h;
  }

  void Destroy(ShaderHandle h)
  {
    Shader& shader = m_shaders[h.id];
    XR_ASSERT(Gfx, shader.refCount > 0);
    --shader.refCount;
    if (shader.refCount == 0)
    {
      XR_GL_CALL(glDeleteShader(shader.name));
      std::memset(&shader, 0x00, sizeof(Shader));

      m_shaders.server.Release(h.id);
    }
  }

  ProgramHandle CreateProgram(ShaderHandle hVertex, ShaderHandle hFragment)
  {
    XR_ASSERT(Gfx, hVertex.id < m_shaders.server.GetNumActive());
    XR_ASSERT(Gfx, hFragment.id < m_shaders.server.GetNumActive());

    // Create program
    Program program;
    XR_GL_CALL(program.name = glCreateProgram());

    Shader& shdVert = m_shaders[hVertex.id];
    Shader& shdFrag = m_shaders[hFragment.id];

    // attempt to attach shaders
    GLint attachResult;
    XR_GL_CALL_RESULT(glAttachShader(program.name, shdVert.name), attachResult);
    if (attachResult == GL_NO_ERROR)
    {
      program.hVertex = hVertex;
      XR_GL_CALL_RESULT(glAttachShader(program.name, shdFrag.name), attachResult);
    }

    bool result = attachResult == GL_NO_ERROR;
    if (result)
    {
      // attempt to link program
      program.hFragment = hFragment;
      XR_GL_CALL(glLinkProgram(program.name));

      GLint params = -1;
      XR_GL_CALL(glGetProgramiv(program.name, GL_LINK_STATUS, &params));
      result = params == GL_TRUE;
      program.linked = result;
      if (!result)
      {
        char log[1024];
        XR_GL_CALL(glGetProgramInfoLog(program.name, sizeof(log), 0, log));
        XR_TRACE(Gfx, ("Failed to link program (hVertex: %d, hFragment: %d): %s",
          hVertex, hFragment, log));
      }
    }

    if (result)
    {
      // attrib / uniform names
      GLint maxLen, maxLenn;
      XR_GL_CALL(glGetProgramiv(program.name, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLen));
      XR_GL_CALL(glGetProgramiv(program.name, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLenn));
      if (maxLenn > maxLen)
      {
        maxLen = maxLenn;
      }
      ++maxLen; // null terminator
      char* nameBuffer = (char*)alloca(maxLen);

      // process attribs
      GLint arraySize;
      GLenum type;

      std::memset(program.attribLoc, 0xff, sizeof(program.attribLoc));
      std::fill(program.activeAttribs, program.activeAttribs + XR_ARRAY_SIZE(program.activeAttribs),
        uint8_t(Attribute::kCount));
      program.ResetAttribBindings();
      //XR_GL_CALL(glGetProgramiv(program.name, GL_ACTIVE_ATTRIBUTES, &program.numActiveAttribs));
      for (int i = 0; i < uint8_t(Attribute::kCount); ++i)
      {
        XR_GL_CALL(GLint loc = glGetAttribLocation(program.name, kAttributeName[i]));
        if (loc != -1)
        {
          program.attribLoc[i] = loc;
          program.activeAttribs[program.numActiveAttribs] = i;
          ++program.numActiveAttribs;
        }
      }

      // TODO: instance data support

      // process uniforms
      GLint num, loc;
      XR_GL_CALL(glGetProgramiv(program.name, GL_ACTIVE_UNIFORMS, &num));
      for (int i = 0; i < num; ++i)
      {
        XR_GL_CALL(glGetActiveUniform(program.name, i, maxLen, NULL, &arraySize, &type, nameBuffer));
        loc = glGetUniformLocation(program.name, nameBuffer);

        auto iFind = m_uniformHandles.find(XR::Hash::String(nameBuffer));
        if (iFind != m_uniformHandles.end())
        {
          Uniform& u = m_uniforms[iFind->second.id];
          ++u.refCount;

          if (!program.uniforms)
          {
            program.uniforms = ConstBuffer::Create(1024);
          }
          program.uniforms->WriteHandle(u.type, u.arraySize, 0, iFind->second);
          program.uniforms->Write(loc);
        }

        XR_TRACE(Gfx, ("Uniform %s: %s[%d] at location %d", nameBuffer, GetGLSLTypeName(type),
          arraySize, loc));
      }
    }

    // finalize program
    ProgramHandle h;
    if (result)
    {
      if (program.uniforms)
      {
        program.uniforms->Finalize();
      }

      ++shdFrag.refCount;
      ++shdVert.refCount;

      h.id = uint16_t(m_programs.server.Acquire());
      m_programs[h.id] = program;
    }
    else
    {
      // failure - delete program
      glDetachShader(program.name, shdVert.name);
      glDetachShader(program.name, shdFrag.name);
      XR_GL_IGNORE_ERROR;

      if(program.uniforms)
      {
        program.ReleaseUniforms(m_uniforms.server.GetNumActive(), m_uniforms.data);
        ConstBuffer::Destroy(program.uniforms);
      }

      XR_GL_CALL(glDeleteProgram(program.name));
      program.name = -1;
    }

    return h;
  }

  void Destroy(ProgramHandle h)
  {
    Program& p = m_programs[h.id];
    XR_GL_CALL(glUseProgram(0));
    XR_GL_CALL(glDeleteProgram(p.name));
    p.name = 0;
    p.linked = false;
    p.hVertex = ShaderHandle();
    p.hFragment = ShaderHandle();
    m_programs.server.Release(h.id);

    if (p.uniforms)
    {
      p.ReleaseUniforms(m_uniforms.server.GetNumActive(), m_uniforms.data);
      ConstBuffer::Destroy(p.uniforms);
    }
    p.uniforms = 0;
  }

  void ClearBuffer(uint32_t flags, Color const& color, float depth, uint8_t stencil)
  {
    GLbitfield flagls = 0;
    if (flags & F_CLEAR_COLOR)
    {
      XR_GL_CALL(glClearColor(color.r, color.g, color.b, color.a));
      flagls |= GL_COLOR_BUFFER_BIT;
    }

    if (flags & F_CLEAR_DEPTH)
    {
      XR_GL_CALL(glClearDepth(depth));
      flagls |= GL_DEPTH_BUFFER_BIT;
    }

    if (flags & F_CLEAR_STENCIL)
    {
      XR_GL_CALL(glClearStencil(stencil));
      flagls |= GL_STENCIL_BUFFER_BIT;
    }

    XR_GL_CALL(glClear(flagls));
  }

  void SetViewport(Rect const& rect)
  {
    XR_GL_CALL(glViewport(rect.x, rect.y, rect.width, rect.height));
  }

  void SetScissor(Rect const& rect)
  {
    XR_GL_CALL(glScissor(rect.x, rect.y, rect.width, rect.height));
  }

  void SetUniform(UniformHandle h, uint8_t num, void const* data)
  {
    Uniform& u = m_uniforms[h.id];
    XR_ASSERT(GFX, u.refCount > 0);
    //m_uniformBuffer->WriteData(u.type, std::min(u.arraySize, num), h.id, data);
    std::memcpy(u.data, data, kUniformTypeSize[uint8_t(u.type)] * std::min(u.arraySize, num));
    // TODO: update timestamp
  }

  void SetTexture(TextureHandle h, uint32_t stage)
  {
    XR_GL_CALL(glActiveTexture(GL_TEXTURE0 + stage));
    Texture& t = m_textures[h.id];
    t.Bind();
  }

  void SetState(uint32_t flags)
  {
    const uint32_t deltaFlags = flags ^ m_activeState;

    // depth test
    if (IsFullMask(deltaFlags, F_STATE_DEPTH_TEST))
    {
      GL::SwitchEnabledState(GL_DEPTH_TEST, IsFullMask(flags, F_STATE_DEPTH_TEST));
    }

    // depth write
    if (IsFullMask(deltaFlags, F_STATE_DEPTH_WRITE))
    {
      XR_GL_CALL(glDepthMask(IsFullMask(flags, F_STATE_DEPTH_WRITE)));
    }

    // blending
    if (IsFullMask(deltaFlags, F_STATE_ALPHA_BLEND))
    {
      bool alphaBlend = IsFullMask(flags, F_STATE_ALPHA_BLEND);
      GL::SwitchEnabledState(GL_BLEND, alphaBlend);
      if (alphaBlend)
      {
        // TODO: improved blend func support
        XR_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
      }
    }

    // culling
    if ((deltaFlags & (F_STATE_CULL_BACK | F_STATE_CULL_FRONT)) != 0)
    {
      uint32_t culling = flags & (F_STATE_CULL_BACK | F_STATE_CULL_FRONT);
      bool hasCulling = culling != 0;
      GL::SwitchEnabledState(GL_CULL_FACE, hasCulling);
      if (hasCulling)
      {
        GLenum cullMode;
        switch (culling)
        {
        case F_STATE_CULL_BACK:
          cullMode = GL_BACK;
          break;

        case F_STATE_CULL_FRONT:
          cullMode = GL_FRONT;
          break;

        default:
          cullMode = GL_FRONT_AND_BACK;
          break;
        }

        XR_GL_CALL(glCullFace(cullMode));
      }
    }

    // scissor test
    if (IsFullMask(deltaFlags, F_STATE_SCISSOR_TEST))
    {
      XR_GL_CALL(GL::SwitchEnabledState(GL_SCISSOR_TEST,
        IsFullMask(flags, F_STATE_SCISSOR_TEST)));
    }

    // wireframe
    if (IsFullMask(deltaFlags, F_STATE_WIREFRAME))
    {
      XR_GL_CALL(glPolygonMode(GL_FRONT_AND_BACK,
        IsFullMask(flags, F_STATE_WIREFRAME) ? GL_LINE : GL_FILL));
    }

    m_activeState = flags;
  }

  void SetProgram(ProgramHandle h)
  {
    Program& program = m_programs[h.id];
    XR_GL_CALL(glUseProgram(program.name)); // TODO: fixed pipeline with program 0 / invalid handle?
    program.UpdateUniforms(m_uniforms.server.GetNumActive(), m_uniforms.data);
    m_activeProgram = h;
  }

  void SetFrameBuffer(FrameBufferHandle h)
  {
    GLint name = h.IsValid() ? m_renderTargets[h.id].name : 0;
    XR_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, name));
  }

  void Draw(VertexBufferHandle vbh, PrimType pt, uint32_t offset, uint32_t count)
  {
    XR_ASSERT(Gfx, m_activeProgram.id != INVALID_ID);
    Program& program = m_programs[m_activeProgram.id];
    XR_ASSERT(Gfx, program.linked == true);

    program.ResetAttribBindings();

    VertexBufferObject const& vbo = m_vbos[vbh.id];
    VertexFormatRef const& vf = m_vertexFormats[vbo.hFormat.id];
    vbo.Bind();
    program.BindAttributes(vf.format, 0);

    XR_GL_CALL(glDrawArrays(kPrimitiveTypes[uint8_t(pt)], offset, count));
    program.UnbindAttributes();
  }

  void Draw(VertexBufferHandle vbh, IndexBufferHandle ibh, PrimType pt, uint32_t offset, uint32_t count)
  {
    XR_ASSERT(Gfx, m_activeProgram.id != INVALID_ID);
    Program& program = m_programs[m_activeProgram.id];
    XR_ASSERT(Gfx, program.linked == true);

    program.ResetAttribBindings();

    VertexBufferObject const& vbo = m_vbos[vbh.id];
    VertexFormatRef const& vf = m_vertexFormats[vbo.hFormat.id];
    vbo.Bind();
    program.BindAttributes(vf.format, 0);

    IndexBufferObject const& ibo = m_ibos[ibh.id];
    ibo.Bind();
    GLenum types[] = { GL_UNSIGNED_SHORT, GL_UNSIGNED_INT };
    XR_GL_CALL(glDrawElementsInstanced(kPrimitiveTypes[uint8_t(pt)], count,
      types[(ibo.flags & F_BUFFER_INDEX_32BITS) != 0],
      reinterpret_cast<void*>(uintptr_t(offset * ibo.indexSize)), 1)); // TODO: instanced drawing
    program.UnbindAttributes();
  }

  void Flush()
  {
    XR_GL_CALL(glFlush());
    m_framePool.Flush();
    //++m_flushId;
  }

  void Present(bool resetState)
  {
    Flush();
    SDL_GL_SwapWindow(m_window);
  }

private:
  // data
  SDL_Window* m_window;
  SDL_Renderer* m_ownRenderer;
  SDL_GLContext m_ownContext;

  SVector2 m_physicalSize;
  SVector2 m_logicalSize;

  ServicedArray<VertexFormatRef, 64> m_vertexFormats;
  std::unordered_map<uint32_t, VertexFormatHandle> m_vertexFormatHandles;

  ServicedArray<VertexBufferObject, 4096> m_vbos;
  ServicedArray<IndexBufferObject, 4096> m_ibos;
  ServicedArray<Texture, 1024> m_textures;
  ServicedArray<FrameBuffer, 256> m_renderTargets;

  ServicedArray<Uniform, 1024> m_uniforms;
  std::unordered_map<uint32_t, UniformHandle> m_uniformHandles;

  ServicedArray<Shader, 512> m_shaders;
  ServicedArray<Program, 512> m_programs;

  GLuint m_vao = 0;
  uint32_t m_activeState = F_STATE_NONE;
  ProgramHandle m_activeProgram;

  Pool m_framePool;
};

//==============================================================================
void ContextDeleter(Context* ctx)
{
  ctx->~Context();
  g_allocator->Deallocate(ctx);
};

std::unique_ptr<Context, std::function<void(Context*)> > s_impl(nullptr, ContextDeleter);

//=============================================================================
void Init(void * window, Allocator* alloc)
{
  auto sdlWindow = static_cast<SDL_Window*>(window);

  if (!alloc)
  {
    static Mallocator mallocator;
    alloc = &mallocator;
  }
  g_allocator = alloc;

  s_impl.reset(new (g_allocator->Allocate(sizeof(Context))) Context(sdlWindow));
}

//=============================================================================
uint16_t GetWidth()
{
  return s_impl->GetWidth();
}

//=============================================================================
uint16_t GetHeight()
{
  return s_impl->GetHeight();
}

//=============================================================================
void Exit()
{
  s_impl.reset();
  g_allocator = nullptr;
}

//=============================================================================
VertexFormatHandle RegisterVertexFormat(VertexFormat const & format)
{
  return s_impl->RegisterVertexFormat(format);
}

//=============================================================================
void Destroy(VertexFormatHandle h)
{
  s_impl->Destroy(h);
}

//=============================================================================
VertexBufferHandle CreateVertexBuffer(VertexFormatHandle hFormat, Buffer const & buffer, uint32_t flags)
{
  return s_impl->CreateVertexBuffer(hFormat, buffer, flags);
}

//=============================================================================
void Destroy(VertexBufferHandle h)
{
  s_impl->Destroy(h);
}

//=============================================================================
IndexBufferHandle CreateIndexBuffer(Buffer const & buffer, uint32_t flags)
{
  return s_impl->CreateIndexBuffer(buffer, flags);
}

//=============================================================================
void Destroy(IndexBufferHandle h)
{
  s_impl->Destroy(h);
}

//=============================================================================
TextureHandle CreateTexture(TextureFormat format, uint32_t width,
  uint32_t height, uint32_t depth, uint32_t flags, Buffer const* buffer,
  size_t numBuffers)
{
  return s_impl->CreateTexture(format, width, height, depth, flags, buffer, numBuffers);
}

//=============================================================================
TextureInfo const& GetTextureInfo(TextureHandle h)
{
  return s_impl->GetTextureInfo(h);
}


//=============================================================================
void Destroy(TextureHandle h)
{
  s_impl->Destroy(h);
}

//=============================================================================
FrameBufferHandle CreateFrameBuffer(TextureFormat format, uint32_t width, uint32_t height,
  uint32_t flags)
{
  return s_impl->CreateFrameBuffer(format, width, height, flags);
}

//=============================================================================
FrameBufferHandle CreateFrameBuffer(uint8_t textureCount, TextureHandle const* hTextures, bool ownTextures)
{
  return s_impl->CreateFrameBuffer(textureCount, hTextures, ownTextures);
}

//=============================================================================
FrameBufferHandle CreateFrameBuffer(uint8_t textureCount, FrameBufferAttachment const * attachments, bool ownTextures)
{
  return s_impl->CreateFrameBuffer(textureCount, attachments, ownTextures);
}

//=============================================================================
void Destroy(FrameBufferHandle h)
{
  s_impl->Destroy(h);
}

//=============================================================================
UniformHandle CreateUniform(char const* name, UniformType type, uint8_t arraySize)
{
  return s_impl->CreateUniform(name, type, arraySize);
}

//=============================================================================
void Destroy(UniformHandle h)
{
  s_impl->Destroy(h);
}

//=============================================================================
ShaderHandle CreateShader(ShaderType t, Buffer const & buffer)
{
  return s_impl->CreateShader(t, buffer);
}

//=============================================================================
void Destroy(ShaderHandle h)
{
  s_impl->Destroy(h);
}

//=============================================================================
ProgramHandle CreateProgram(ShaderHandle hVertex, ShaderHandle hFragment)
{
  return s_impl->CreateProgram(hVertex, hFragment);
}

//=============================================================================
void Destroy(ProgramHandle h)
{
  s_impl->Destroy(h);
}

//=============================================================================
void Clear(uint32_t flags, Color color, float depth, uint8_t stencil)
{
  s_impl->ClearBuffer(flags, color, depth, stencil);
}

//==============================================================================
void SetViewport(Rect const& rect)
{
  s_impl->SetViewport(rect);
}

//==============================================================================
void SetScissor(Rect const& rect)
{
  s_impl->SetScissor(rect);
}

//==============================================================================
void SetUniform(UniformHandle h, uint8_t num, void const* data)
{
  s_impl->SetUniform(h, num, data);
}

//==============================================================================
void SetTexture(TextureHandle h, uint32_t stage)
{
  XR_ASSERT(Gfx, stage < kMaxTextureStages);
  s_impl->SetTexture(h, stage);
}

//==============================================================================
void SetState(uint32_t flags)
{
  s_impl->SetState(flags);
}

//==============================================================================
void SetProgram(ProgramHandle h)
{
  s_impl->SetProgram(h);
}

//==============================================================================
void SetFrameBuffer(FrameBufferHandle h)
{
  s_impl->SetFrameBuffer(h);
}

//==============================================================================
void Draw(VertexBufferHandle vbh, PrimType primitiveType, uint32_t offset, uint32_t count)
{
  s_impl->Draw(vbh, primitiveType, offset, count);
}

//=============================================================================
void Draw(VertexBufferHandle vbh, IndexBufferHandle ibh, PrimType primitiveType, uint32_t offset, uint32_t count)
{
  s_impl->Draw(vbh, ibh, primitiveType, offset, count);
}

//==============================================================================
void Flush()
{
  s_impl->Flush();
}

//==============================================================================
void Present(bool resetState)
{
  s_impl->Present(resetState);
}

}
}

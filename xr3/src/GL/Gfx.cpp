//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Gfxp.hpp"
#include "xrgl.hpp"
#include "xr/Matrix.hpp"
#include "xr/Device.hpp"
#include "xr/SVector2.hpp"
#include "xr/Pool.hpp"
#include "xr/Hash.hpp"
#include "gl_core_4_5.h"
#include "GfxContext.hpp"

#include <set>
#include <unordered_map>
#include <functional>

namespace xr
{
namespace Gfx
{
namespace
{

#define UINT_PTR_CAST(x) reinterpret_cast<void*>(static_cast<uintptr_t>(x))

//=============================================================================
static TextureHandle kDefaultTexture2D;
static TextureHandle kDefaultTexture3D;
static TextureHandle kDefaultTextureCube;
static FrameBufferHandle kDefaultFrameBuffer;

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
char const *const  kInstanceDataName[] =
{
  "iData0",
  "iData1",
  "iData2",
  "iData3",
};
static_assert(XR_ARRAY_SIZE(kInstanceDataName) == kMaxInstanceData,
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
};

//=============================================================================
const GLenum kBlendFactors[] =
{
  GLenum(-1),
  GL_ZERO,
  GL_ONE,
  GL_SRC_COLOR,
  GL_ONE_MINUS_SRC_COLOR,
  GL_SRC_ALPHA,
  GL_ONE_MINUS_SRC_ALPHA,
  GL_DST_COLOR,
  GL_ONE_MINUS_DST_COLOR,
  GL_DST_ALPHA,
  GL_ONE_MINUS_DST_ALPHA,
};

void GetBlendFactors(FlagType flags, GLenum blendFactors[4])
{
  BlendFactor::Type f = (flags >> F_STATE_BLENDF_BASE_SHIFT) & 0xf;
  XR_ASSERT(Gfx, f < XR_ARRAY_SIZE(kBlendFactors));
  blendFactors[0] = kBlendFactors[f];

  f = (flags >> (F_STATE_BLENDF_BASE_SHIFT + F_STATE_BLENDF_ALPHA_SHIFT)) & 0xf;
  XR_ASSERT(Gfx, f < XR_ARRAY_SIZE(kBlendFactors));
  blendFactors[1] = kBlendFactors[f];

  f = (flags >> (F_STATE_BLENDF_BASE_SHIFT + F_STATE_BLENDF_DST_SHIFT)) & 0xf;
  XR_ASSERT(Gfx, f < XR_ARRAY_SIZE(kBlendFactors));
  blendFactors[2] = kBlendFactors[f];

  f = (flags >> (F_STATE_BLENDF_BASE_SHIFT + F_STATE_BLENDF_DST_SHIFT +
    F_STATE_BLENDF_ALPHA_SHIFT)) & 0xf;
  XR_ASSERT(Gfx, f < XR_ARRAY_SIZE(kBlendFactors));
  blendFactors[3] = kBlendFactors[f];
}

//=============================================================================
const GLenum kComparisons[] =
{
  GLenum(-1),
  GL_NEVER,
  GL_LESS,
  GL_LEQUAL,
  GL_EQUAL,
  GL_GEQUAL,
  GL_GREATER,
  GL_NOTEQUAL,
  GL_ALWAYS
};

GLenum GetDepthFunc(FlagType flags)
{
  auto c = (flags >> F_STATE_DEPTH_COMPF_SHIFT) & 0xf;
  XR_ASSERT(Gfx, c < XR_ARRAY_SIZE(kComparisons));
  return kComparisons[c];
}

constexpr GLenum GetStencilFunc(FlagType flags)
{
  return kComparisons[(flags & F_STENCIL_COMPF_MASK) >> F_STENCIL_COMPF_SHIFT];
}

//=============================================================================
const GLenum kStencilOps[] =
{
  GLenum(-1),
  GL_KEEP,
  GL_ZERO,
  GL_INCR,
  GL_INCR_WRAP,
  GL_DECR,
  GL_DECR_WRAP,
  GL_INVERT,
  GL_REPLACE
};

void GetStencilOps(FlagType flags, GLenum ops[3])
{
  ops[0] = kStencilOps[(flags & F_STENCIL_FAIL_OP_MASK) >> F_STENCIL_FAIL_OP_SHIFT];
  ops[1] = kStencilOps[(flags & F_STENCIL_DEPTH_FAIL_OP_MASK) >> F_STENCIL_DEPTH_FAIL_OP_SHIFT];
  ops[2] = kStencilOps[(flags & F_STENCIL_ALL_PASS_OP_MASK) >> F_STENCIL_ALL_PASS_OP_SHIFT];
}

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
using VertexFormatRef = Ref<VertexFormat>;

//=============================================================================
struct ResourceGL
{
  GLuint name = -1;
};

//=============================================================================
struct VertexBufferObject : ResourceGL
{
  static uint32_t EncodeInstanceDataStride(uint16_t stride)
  {
    return F_BUFFER_INSTANCE_DATA |
      (stride << (XR_BITSIZEOF(FlagType) - (1 + XR_BITSIZEOF(stride))));
  }

  VertexFormatHandle hFormat;
  FlagType flags = F_BUFFER_NONE;
  GLenum  target;

  void Bind() const
  {
    XR_GL_CALL(glBindBuffer(target, name));
  }

  uint16_t DecodeInstanceDataStride() const
  {
    XR_ASSERT(Gfx, CheckAllMaskBits(flags, F_BUFFER_INSTANCE_DATA));
    return (flags & ~F_BUFFER_INSTANCE_DATA) >>
      (XR_BITSIZEOF(FlagType) - (1 + XR_BITSIZEOF(uint16_t)));
  }
};

//=============================================================================
struct IndexBufferObject : ResourceGL
{
  uint8_t indexSize = 0;
  FlagType flags = F_BUFFER_NONE;
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
    uint16_t attachmentTypeBits;
    bool compressed;
  };

  TextureInfo info;
  GLenum target = GL_NONE;

  void Bind()
  {
    XR_GL_CALL(glBindTexture(target, name));
  }
};

using TextureRef = Ref<Texture>;

// Pack color / depth / stencil bits.
#define PACK_TEX_COMP(component, size) ((size) & 0x3f) << (2 + ((component) * 6))

const Texture::Format kTextureFormats[] =
{
  { GL_R8, GL_ZERO, GL_RED, GL_UNSIGNED_BYTE,
    uint8_t(AttachmentType::Color) | PACK_TEX_COMP(0, 8), false },
  { GL_RGB565, GL_ZERO, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
    uint8_t(AttachmentType::Color) | PACK_TEX_COMP(0, 16), false },
  { GL_RGB8, GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE,
    uint8_t(AttachmentType::Color) | PACK_TEX_COMP(0, 24), false },
  { GL_RGBA8, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE,
    uint8_t(AttachmentType::Color) | PACK_TEX_COMP(0, 32), false },
  { GL_RGBA8, GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_BYTE,
    uint8_t(AttachmentType::Color) | PACK_TEX_COMP(0, 32), false },

  // compressed
  // depth/stencil
  { GL_DEPTH_COMPONENT32, GL_ZERO, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT,
    uint8_t(AttachmentType::Depth) | PACK_TEX_COMP(0, 32), false },
  { GL_DEPTH24_STENCIL8, GL_ZERO, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,
    uint8_t(AttachmentType::DepthStencil) | PACK_TEX_COMP(0, 24) | PACK_TEX_COMP(1, 8), false },
  { GL_STENCIL_INDEX8, GL_ZERO, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE,
    uint8_t(AttachmentType::Stencil) | PACK_TEX_COMP(0, 8), false },
};
static_assert(XR_ARRAY_SIZE(kTextureFormats) == size_t(TextureFormat::kCount),
  "Count of texture formats / definition must match.");

#undef PACK_TEX_COMP

//=============================================================================
struct FrameBuffer : ResourceGL
{
  TextureHandle hTextures[8];
  uint8_t numTextures;
  uint8_t numColorAttachments;
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
};

using ShaderRef = Ref<Shader>;

//=============================================================================
struct Program: ResourceGL
{
  ShaderHandle  hVertex;
  ShaderHandle  hFragment;
  bool          linked = false;
  int           numActiveAttribs = 0;
  uint8_t       activeAttribs[XR_ARRAY_SIZE(kAttributeName)]; // ordinal to XR attrib id
  GLint         attribLoc[XR_ARRAY_SIZE(kAttributeName)]; // XR attrib id to GLSL location
  uint8_t       unboundAttribs[XR_ARRAY_SIZE(kAttributeName)]; // bound from VertexFormat
  GLint         instanceData[XR_ARRAY_SIZE(kInstanceDataName) + 1]; // additional space for -1 terminator
  ConstBuffer*  uniforms = nullptr;

  void UpdateUniforms(size_t numUniforms, void const* const* uniformData)
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
        void const* value = uniformData[hUniform.id];
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

        default:
          break;
        }
      }
    }
  }

  void ReleaseUniforms(size_t numUniforms, UniformRef* uniformData)
  {
    uniforms->Reset();

    UniformType ignoreType;
    uint8_t ignoreSize;
    uint16_t ignoreLoc;
    UniformHandle hUniform;
    GLint ignoreGlLoc;
    while (!uniforms->AtEnd())
    {
      uniforms->ReadHandle(ignoreType, ignoreSize, ignoreLoc, hUniform);
      XR_ASSERT(Gfx, hUniform.id < numUniforms);
      Release(hUniform);
      uniforms->Read(ignoreGlLoc);
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
        XR_GL_CALL(glVertexAttribPointer(loc, numComponents, GL_FLOAT, normalized, format.GetStride(), UINT_PTR_CAST(attribBase)));

        unboundAttribs[i] = uint8_t(Attribute::kCount);
      }
    }
  }

  void UnbindUnusedAttributes() const
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

  void BindInstanceData(uint16_t stride, uint16_t baseInstance) const
  {
    uint16_t offset = baseInstance * stride;
    for (int i = 0; instanceData[i] != -1; ++i)
    {
      GLint loc = instanceData[i];
      XR_GL_CALL(glEnableVertexAttribArray(loc));
      XR_GL_CALL(glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, stride,
        UINT_PTR_CAST(offset)));
      XR_GL_CALL(glVertexAttribDivisor(loc, 1));
      offset += 4 * sizeof(float);
    }
  }

  void UnbindInstanceData() const
  {
    for (int i = 0; instanceData[i] != -1; ++i)
    {
      GLint loc = instanceData[i];
      XR_GL_CALL(glDisableVertexAttribArray(loc));
    }
  }
};

//=============================================================================
#ifdef XR_DEBUG
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
#endif

//=============================================================================
struct Impl
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
        extensions.insert({ span, (uint8_t const*)ext });

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
        extensions.insert({ strlen(ext), (uint8_t const*)ext });
      }
    }

    // check support now
    for (auto& i : s_extensions)
    {
      i.supported = extensions.end() !=
        extensions.find({ strlen(i.name), (uint8_t const*)i.name });
    }
  }

  Impl(Context* context)
  : m_context(context)
  {
    XR_TRACE(Gfx, ("Gfx init start..."));
    m_context->Init();

    // Load GL functions. We're aiming quite high with the version number, simply
    // because functionality has only been added to OpenGL (at least since 3.3)
    // and because we should. Requesting a lower version context will likely lead
    // to a rise in the number of load failures.
    // TODO: Test properly + find a way to determine if anything crucial is unavailable.
    auto numFailed = ogl_LoadFunctions();
    if (numFailed > 0)
    {
      XR_TRACE(Gfx, ("Failed to load %d functions.", numFailed));
    }

    // log vendor, renderer, driver version
    XR_TRACE(Gfx, ("Vendor: %s", glGetString(GL_VENDOR)));
    XR_TRACE(Gfx, ("Renderer: %s", glGetString(GL_RENDERER)));
    XR_TRACE(Gfx, ("Version: %s", glGetString(GL_VERSION)));

    // initialise extensions
    InitExtensions(context->GetGlVersionMajor() * 10 + context->GetGlVersionMinor());

    SetViewport({ 0, 0, GetLogicalWidth(), GetLogicalHeight() });

    m_activeState = (m_activeState & ~F_STATE_DEPTH_COMPF_MASK) |
      DepthFuncToState(Comparison::LEQUAL);
    XR_GL_CALL(glDepthFunc(GL_LEQUAL));

    m_activeState = (m_activeState & ~F_STATE_BLENDF_MASK) |
      BlendFactorToState(BlendFactor::SRC_ALPHA, BlendFactor::INV_SRC_ALPHA);
    XR_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    const FlagType activeStencil = StencilToState(0x00, 0xff, Comparison::FAIL,
      StencilOp::KEEP, StencilOp::KEEP, StencilOp::KEEP);
    m_activeStencil[0] = activeStencil;
    m_activeStencil[1] = activeStencil;
    XR_GL_CALL(glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_NEVER, 0x00, 0xff));
    XR_GL_CALL(glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP));

    if (context->GetGlVersionMajor() >= 3 ||
      s_extensions[ExtensionGL::OES_vertex_array_object].supported ||
      s_extensions[ExtensionGL::ARB_vertex_array_object].supported)
    {
      XR_GL_CALL(glGenVertexArrays(1, &m_vao));
      XR_GL_CALL(glBindVertexArray(m_vao));
    }

    // create default textures and framebuffer
    kDefaultTexture2D.id = decltype(m_textures)::kSize - 1;
    CreateDefaultTexture(kDefaultTexture2D, GL_TEXTURE_2D);

    kDefaultTexture3D.id = decltype(m_textures)::kSize - 2;
    CreateDefaultTexture(kDefaultTexture3D, GL_TEXTURE_3D);

    kDefaultTextureCube.id = decltype(m_textures)::kSize - 3;
    CreateDefaultTexture(kDefaultTextureCube, GL_TEXTURE_CUBE_MAP);

    kDefaultFrameBuffer.id = decltype(m_fbos)::kSize - 1;
    auto& defaultFbo = m_fbos[kDefaultFrameBuffer.id];
    defaultFbo.numColorAttachments = 2;
    defaultFbo.name = 0;

    XR_TRACE(Gfx, ("Gfx init complete."));
  }

  ~Impl()
  {
    XR_TRACE(Gfx, ("Gfx shutting down."));
    for (auto& cb: m_onExit)
    {
      cb.Call(nullptr);
    }

    if (m_vao != 0)
    {
      XR_GL_CALL(glDeleteVertexArrays(1, &m_vao));
    }

#define REPORT_LEAKS(name) XR_TRACEIF(Gfx, m_##name##s.server.GetNumAcquired() > 0,\
  ("WARNING: %d %s handles leaked.", m_##name##s.server.GetNumAcquired(), #name))
    REPORT_LEAKS(vertexFormat);
    REPORT_LEAKS(vbo);
    REPORT_LEAKS(ibo);
    REPORT_LEAKS(texture);
    REPORT_LEAKS(fbo);
    REPORT_LEAKS(uniform);
    REPORT_LEAKS(shader);
    REPORT_LEAKS(program);
#undef REPORT_LEAKS

    XR_TRACE(Gfx, ("Gfx shutdown complete."));
  }

  int GetLogicalWidth() const
  {
    return m_context->GetLogicalWidth();
  }

  int GetLogicalHeight() const
  {
    return m_context->GetLogicalHeight();
  }

  int GetPhysicalWidth() const
  {
    return m_context->GetPhysicalWidth();
  }

  int GetPhysicalHeight() const
  {
    return m_context->GetPhysicalHeight();
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
      VertexFormatRef& vfr = m_vertexFormats[hFormat.id];
      vfr.inst = format;
      vfr.refCount = 1;
    }
    return hFormat;
  }

  void Release(VertexFormatHandle h)
  {
    VertexFormatRef& vfr = m_vertexFormats[h.id];
    XR_ASSERT(Gfx, vfr.refCount > 0);
    --vfr.refCount;
    if (vfr.refCount == 0)
    {
      uint32_t hash = vfr.inst.CalculateHash();
      m_vertexFormatHandles.erase(hash);

      m_vertexFormats.server.Release(h.id);
    }
  }

  VertexBufferHandle CreateVertexBuffer(VertexFormatHandle hFormat, Buffer const& buffer, FlagType flags)
  {
    XR_ASSERT(Gfx, hFormat.IsValid());
    ++m_vertexFormats[hFormat.id].refCount;

    XR_TRACEIF(Gfx, CheckAllMaskBits(flags, F_BUFFER_INSTANCE_DATA),
      ("Ignored instance data buffer request in CreateVertexBuffer()."));
    return CreateVertexBufferInternal(hFormat, buffer, flags & ~F_BUFFER_INSTANCE_DATA);
  }

  void Release(VertexBufferHandle h)
  {
    VertexBufferObject& vbo = m_vbos[h.id];
    XR_GL_CALL(glBindBuffer(vbo.target, 0));
    XR_GL_CALL(glDeleteBuffers(1, &vbo.name));

    if (vbo.hFormat.IsValid())  // instance data buffers don't have this.
    {
      Release(vbo.hFormat);
    }

    std::memset(&vbo, 0x00, sizeof(vbo));
    m_vbos.server.Release(h.id);
  }

  IndexBufferHandle CreateIndexBuffer(Buffer const& buffer, FlagType flags)
  {
    IndexBufferHandle h;
    h.id = uint16_t(m_ibos.server.Acquire());

    IndexBufferObject& ibo = m_ibos[h.id];
    XR_GL_CALL(glGenBuffers(1, &ibo.name));
    ibo.Bind();
    XR_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.size, buffer.data,
      buffer.data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW));

    ibo.indexSize = CheckAllMaskBits(flags, F_BUFFER_INDEX_32BITS) ? sizeof(uint32_t) : sizeof(uint16_t);
    ibo.flags = flags;

    return h;
  }

  void Release(IndexBufferHandle h)
  {
    IndexBufferObject& ibo = m_ibos[h.id];
    XR_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    XR_GL_CALL(glDeleteBuffers(1, &ibo.name));

    std::memset(&ibo, 0x00, sizeof(ibo));
    m_ibos.server.Release(h.id);
  }

  InstanceDataBufferHandle CreateInstanceDataBuffer(Buffer const& buffer, uint16_t stride)
  {
    XR_ASSERT(Gfx, buffer.data);
    XR_ASSERTMSG(Gfx, (stride & 0xf) == 0,
      ("16x bytes required for stride, got: %d", stride));

    // Invalid vertexformat (and GL_ARRAY_BUFFER target) signify a vbo used
    // for instance data.

    // Encode stride in flags.
    FlagType flags = VertexBufferObject::EncodeInstanceDataStride(stride);

    InstanceDataBufferHandle h;
    h.id = CreateVertexBufferInternal(VertexFormatHandle(), buffer, flags).id;

    return h;
  }

  void Release(InstanceDataBufferHandle h)
  {
    VertexBufferHandle hConv;
    hConv.id = h.id;
    Release(hConv);
  }

  TextureHandle CreateTexture(TextureFormat format, uint32_t width,
    uint32_t height, uint32_t depth, FlagType flags, Buffer const* buffer,
    size_t numBuffers)
  {
    XR_ASSERT(Gfx, width > 0);
    XR_ASSERT(Gfx, height > 0);
    XR_ASSERT(Gfx, numBuffers > 0);

    Texture t;
    XR_GL_CALL(glGenTextures(1, &t.name));
    t.info.format = format;
    t.info.width = width;
    t.info.height = height;
    t.info.depth = depth;
    t.info.flags = flags;

    // determine target
    t.target = CheckAllMaskBits(flags, F_TEXTURE_CUBE) ? GL_TEXTURE_CUBE_MAP :
      (depth > 0 ? GL_TEXTURE_3D : GL_TEXTURE_2D);
    XR_ASSERT(Gfx, t.target != GL_TEXTURE_CUBE_MAP || (numBuffers % 6) == 0);

    // bind it
    t.Bind();

    GLenum  wrapMode(CheckAllMaskBits(flags, F_TEXTURE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_WRAP_S, wrapMode));
    XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_WRAP_T, wrapMode));
    if (t.target == GL_TEXTURE_3D)
    {
      XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_WRAP_R, wrapMode));
    }

    GLenum  filterMode(CheckAllMaskBits(flags, F_TEXTURE_FILTER_POINT) ? GL_NEAREST : GL_LINEAR);
    XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_MIN_FILTER, filterMode));
    XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_MAG_FILTER, filterMode));

    auto& formatDesc = kTextureFormats[static_cast<int>(format)];
    GLenum fmtInternal = (CheckAllMaskBits(flags, F_TEXTURE_SRGB) && formatDesc.srgbInternal != GL_ZERO) ?
      formatDesc.srgbInternal : formatDesc.internal;

    GLenum target = t.target != GL_TEXTURE_CUBE_MAP ? t.target : GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    const int sides = t.target != GL_TEXTURE_CUBE_MAP ? 1 : 6;
    const int mipMapLevels(CheckAllMaskBits(flags, F_TEXTURE_MIPMAPS) ?
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
            XR_GL_CALL(glCompressedTexImage3D(target, i, fmtInternal, width, height,
              depth, 0, static_cast<GLsizei>(buffer->size), buffer->data));
            break;

          case GL_TEXTURE_2D:
          case GL_TEXTURE_CUBE_MAP:
            XR_GL_CALL(glCompressedTexImage2D(target + j, i, fmtInternal,
              width, height, 0, static_cast<GLsizei>(buffer[j].size), buffer[j].data));
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

    TextureHandle h;
    h.id = uint16_t(m_textures.server.Acquire());

    TextureRef& tr = m_textures[h.id];
    tr.inst = t;
    tr.refCount = 1;
    return h;
  }

  TextureInfo GetTextureInfo(TextureHandle h)
  {
    return m_textures[h.id].inst.info;
  }

  void Release(TextureHandle h)
  {
    TextureRef& texture = m_textures[h.id];
    if(texture.inst.name != 0)
    {
      XR_ASSERT(Gfx, texture.refCount > 0);
      --texture.refCount;
      if (texture.refCount == 0)
      {
        XR_GL_CALL(glDeleteTextures(1, &texture.inst.name));
        std::memset(&texture.inst, 0x00, sizeof(Texture));

        m_textures.server.Release(h.id);
      }
    }
  }

  FrameBufferHandle CreateFrameBuffer(TextureFormat format, uint32_t width, uint32_t height,
    FlagType flags)
  {
    TextureHandle h = Gfx::CreateTexture(format, width, height, 0, flags);
    return CreateFrameBuffer(1, &h, true);
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
    FrameBuffer fbo;

    XR_GL_CALL(glGenFramebuffers(1, &fbo.name));
    fbo.Bind();

    fbo.numTextures = textureCount;
    GLenum colorAttachments[XR_ARRAY_SIZE(FrameBuffer::hTextures)];
    int numColorAttachments = 0;
    for (uint8_t i = 0; i < textureCount; ++i)
    {
      FrameBufferAttachment const& att = attachments[i];
      fbo.hTextures[i] = att.hTexture;

      TextureRef& texture = m_textures[att.hTexture.id];
      XR_ASSERT(Gfx, texture.inst.info.format != TextureFormat::kCount);
      uint16_t bits = kTextureFormats[uint8_t(texture.inst.info.format)].attachmentTypeBits;
      GLenum attachmentType = kAttachmentTypes[bits & 0x3];
      if (attachmentType == GL_COLOR_ATTACHMENT0)
      {
        attachmentType += numColorAttachments;
        colorAttachments[numColorAttachments] = attachmentType;
        ++numColorAttachments;
      }

      GLenum target = texture.inst.target;
      if (GL_TEXTURE_CUBE_MAP == target)
      {
        target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + att.side;
      }

      XR_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, target,
        texture.inst.name, att.mipLevel));

      if (!ownTextures)
      {
        ++texture.refCount;
      }
    }

    XR_GL_CALL(glDrawBuffers(numColorAttachments, colorAttachments));
    fbo.numColorAttachments = numColorAttachments;

    FrameBufferHandle h;
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (GL_FRAMEBUFFER_COMPLETE == status)
    {
      h.id = uint16_t(m_fbos.server.Acquire());

      m_fbos[h.id] = fbo;
    }
    else
    {
      XR_TRACE(Gfx, ("Failed to create framebuffer, status: 0x%x", status));

      if (!ownTextures)
      {
        for (uint8_t i = 0; i < textureCount; ++i)
        {
          Release(attachments[i].hTexture);
        }
      }
    }

    return h;
  }

  void ReadFrameBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
    TextureFormat format, void* mem)
  {
    ReadFrameBuffer(x, y, width, height, format, 0, mem);
  }

  void ReadFrameBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
    TextureFormat format, uint16_t colorAttachment, void* mem)
  {
    XR_ASSERT(Gfx, !m_activeFrameBuffer.IsValid() ||
      m_fbos[m_activeFrameBuffer.id].numColorAttachments > colorAttachment);
    Texture::Format const& texFormat = kTextureFormats[int(format)];
    GLenum formatGl = texFormat.format;
    GLenum type = texFormat.type;
    XR_GL_CALL(glReadBuffer(m_activeFrameBuffer.IsValid() ?
      GL_COLOR_ATTACHMENT0 + colorAttachment : GL_FRONT + colorAttachment));
    XR_GL_CALL(glReadPixels(x, y, width, height, formatGl, type, mem));
  }

  void Release(FrameBufferHandle h)
  {
    FrameBuffer& fbo = m_fbos[h.id];
    fbo.Bind();

    for (uint8_t i = 0; i < fbo.numTextures; ++i)
    {
      Release(fbo.hTextures[i]);
    }
    std::memset(&fbo, 0x00, sizeof(FrameBuffer));

    XR_GL_CALL(glDeleteFramebuffers(1, &fbo.name));
    m_fbos.server.Release(h.id);
  }

  UniformHandle CreateUniform(char const* name, UniformType type, uint8_t arraySize)
  {
    uint32_t const hash = Hash::String32(name);
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

    UniformRef& ur = m_uniforms[h.id];
    XR_ASSERTMSG(Gfx, !existing || type == ur.inst.type,
      ("Uniform '%s' type (%d) mismatches already registered type (%d)", name,
        ur.inst.type, type));
    XR_ASSERTMSG(Gfx, !existing || arraySize <= ur.inst.arraySize,
      ("Uniform '%s' array size (%d) exceeds registered size (%d).", name, arraySize,
        ur.inst.arraySize));
    if (!existing)
    {
      Uniform u;
      u.name = name;
      u.type = type;
      u.arraySize = std::max(arraySize, static_cast<decltype(arraySize)>(1));

      size_t requiredBytes = u.arraySize * kUniformTypeSize[uint8_t(u.type)];
      void*& data = m_uniformData[h.id];
      data = malloc(requiredBytes);
      std::memset(data, 0x00, requiredBytes);

      ur.inst = u;
      ur.refCount = 1;
      m_uniformHandles[hash] = h;
    }
    else
    {
      ++ur.refCount;
    }

    return h;
  }

  void Release(UniformHandle h)
  {
    UniformRef& ur = m_uniforms[h.id];
    XR_ASSERT(Gfx, ur.refCount > 0);
    --ur.refCount;
    if (ur.refCount == 0)
    {
      free(m_uniformData[h.id]);
      m_uniformData[h.id] = nullptr;

      std::memset(&ur.inst, 0x00, sizeof(Uniform));

      m_uniforms.server.Release(h.id);
    }
  }

  ShaderHandle CreateShader(ShaderType type, Buffer const& buffer)
  {
    Shader shader;

    GLenum typegl = kShaderTypes[size_t(type)];
    XR_GL_CALL(shader.name = glCreateShader(typegl));

    XR_ASSERT(Gfx, buffer.size <= static_cast<size_t>(std::numeric_limits<GLint>::max()));
    auto len = static_cast<GLint>(buffer.size);
    XR_GL_CALL(glShaderSource(shader.name, 1, reinterpret_cast<GLchar const* const*>(&buffer.data), &len));
    XR_GL_CALL(glCompileShader(shader.name));

    GLint params = -1;
    XR_GL_CALL(glGetShaderiv(shader.name, GL_COMPILE_STATUS, &params));
    bool success = params == GL_TRUE;
    ShaderHandle h;
    if (success)
    {
      h.id = uint16_t(m_shaders.server.Acquire());

      ShaderRef& sr = m_shaders[h.id];
      sr.inst = shader;
      sr.refCount = 1;
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

  void Release(ShaderHandle h)
  {
    ShaderRef& sr = m_shaders[h.id];
    XR_ASSERT(Gfx, sr.refCount > 0);
    --sr.refCount;
    if (sr.refCount == 0)
    {
      XR_GL_CALL(glDeleteShader(sr.inst.name));
      std::memset(&sr.inst, 0x00, sizeof(Shader));

      m_shaders.server.Release(h.id);
    }
  }

  ProgramHandle CreateProgram(ShaderHandle hVertex, ShaderHandle hFragment)
  {
    // Create program
    Program program;
    XR_GL_CALL(program.name = glCreateProgram());

    ShaderRef& shdVert = m_shaders[hVertex.id];
    ShaderRef& shdFrag = m_shaders[hFragment.id];

    // attempt to attach shaders
    GLint attachResult;
    XR_GL_CALL_RESULT(glAttachShader(program.name, shdVert.inst.name), attachResult);
    if (attachResult == GL_NO_ERROR)
    {
      program.hVertex = hVertex;
      XR_GL_CALL_RESULT(glAttachShader(program.name, shdFrag.inst.name), attachResult);
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
      std::fill(program.activeAttribs, program.activeAttribs +
        XR_ARRAY_SIZE(program.activeAttribs), uint8_t(Attribute::kCount));
      program.ResetAttribBindings();
      //XR_GL_CALL(glGetProgramiv(program.name, GL_ACTIVE_ATTRIBUTES, &program.numActiveAttribs));
      for (int i = 0; i < uint8_t(Attribute::kCount); ++i)
      {
        XR_GL_CALL(GLint loc = glGetAttribLocation(program.name, kAttributeName[i]));
        if (loc != -1)
        {
          program.attribLoc[i] = loc;
          program.activeAttribs[program.numActiveAttribs] = i;
          XR_TRACE(Gfx, ("Attribute %s at location %d", kAttributeName[i], loc));

          ++program.numActiveAttribs;
        }
      }

      size_t used = 0;
      for (int i = 0; i < kMaxInstanceData; ++i)
      {
        XR_GL_CALL(GLint loc = glGetAttribLocation(program.name, kInstanceDataName[i]));
        if (loc != -1)
        {
          program.instanceData[used] = loc;
          XR_TRACE(Gfx, ("Attribute %s at location %d", kInstanceDataName[i], loc));

          ++used;
        }
      }
      program.instanceData[used] = -1;

      // process uniforms
      GLint num, loc;
      XR_GL_CALL(glGetProgramiv(program.name, GL_ACTIVE_UNIFORMS, &num));
      for (int i = 0; i < num; ++i)
      {
        XR_GL_CALL(glGetActiveUniform(program.name, i, maxLen, NULL, &arraySize, &type, nameBuffer));
        loc = glGetUniformLocation(program.name, nameBuffer);

        auto iFind = m_uniformHandles.find(Hash::String32(nameBuffer));
        if (iFind != m_uniformHandles.end())
        {
          UniformRef& ur = m_uniforms[iFind->second.id];
          ++ur.refCount;

          if (!program.uniforms)
          {
            program.uniforms = ConstBuffer::Create(1024);
          }
          program.uniforms->WriteHandle(ur.inst.type, ur.inst.arraySize, 0, iFind->second);
          program.uniforms->Write(loc);

          XR_TRACE(Gfx, ("Uniform %s: %s[%d] at location %d", nameBuffer, GetGLSLTypeName(type),
            arraySize, loc));
        }
        else
        {
          XR_TRACE(Gfx, ("WARNING: ignored unregistered uniform '%s' (%s[%d]).", nameBuffer,
            GetGLSLTypeName(type), arraySize));
        }
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
      glDetachShader(program.name, shdVert.inst.name);
      glDetachShader(program.name, shdFrag.inst.name);
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

  void Release(ProgramHandle h)
  {
    Program& p = m_programs[h.id];
    XR_GL_CALL(glUseProgram(0));
    XR_GL_CALL(glDeleteProgram(p.name));
    p.name = 0;
    p.linked = false;

    if (p.hVertex.IsValid())
    {
      Release(p.hVertex);
      p.hVertex.Invalidate();
    }

    if (p.hFragment.IsValid())
    {
      Release(p.hFragment);
      p.hFragment.Invalidate();
    }

    if (p.uniforms)
    {
      p.ReleaseUniforms(m_uniforms.server.GetNumActive(), m_uniforms.data);
      ConstBuffer::Destroy(p.uniforms);
    }
    p.uniforms = nullptr;

    m_programs.server.Release(h.id);
  }

  void ClearBuffer(FlagType flags, Color const& color, float depth, uint8_t stencil)
  {
    GLbitfield flagls = 0;
    if (CheckAllMaskBits(flags, F_CLEAR_COLOR))
    {
      XR_GL_CALL(glClearColor(color.r, color.g, color.b, color.a));
      flagls |= GL_COLOR_BUFFER_BIT;
    }

    bool tempDepthWrite = false;
    if (CheckAllMaskBits(flags, F_CLEAR_DEPTH))
    {
      // States in Gfx are understood as states for drawing. With OpenGL, we
      // do have to temporarily enable depth writes to clear the depth buffer,
      // even if it wasn't the state set.
      tempDepthWrite = !CheckAllMaskBits(m_activeState, F_STATE_DEPTH_WRITE);
      if (tempDepthWrite)
      {
        XR_GL_CALL(glDepthMask(GL_TRUE));
      }
      XR_GL_CALL(glClearDepth(depth));
      flagls |= GL_DEPTH_BUFFER_BIT;
    }

    if (CheckAllMaskBits(flags, F_CLEAR_STENCIL))
    {
      XR_GL_CALL(glClearStencil(stencil));
      flagls |= GL_STENCIL_BUFFER_BIT;
    }

    XR_GL_CALL(glClear(flagls));
    if (tempDepthWrite)
    {
      XR_GL_CALL(glDepthMask(GL_FALSE));
    }
  }

  void SetViewport(Rect const& rect)
  {
    XR_GL_CALL(glViewport(rect.x, rect.y, rect.w, rect.h));
  }

  void SetScissor(Rect const* rect)
  {
    bool state = rect != nullptr;
    gl::SwitchEnabledState(GL_SCISSOR_TEST, state);
    if (state)
    {
      XR_GL_CALL(glScissor(rect->x, rect->y, rect->w, rect->h));
    }
  }

  void SetUniform(UniformHandle h, uint8_t elem, void const* data)
  {
    UniformRef& ur = m_uniforms[h.id];
    XR_ASSERT(GFX, ur.refCount > 0);
    elem = std::min(ur.inst.arraySize, elem);
    size_t bytes = kUniformTypeSize[uint8_t(ur.inst.type)] * elem;
    std::memcpy(m_uniformData[h.id], data, bytes);
  }

  void SetTexture(TextureHandle h, uint32_t stage)
  {
    if (m_activeTextures[stage] != h)
    {
      m_activeTextures[stage] = h;
      XR_GL_CALL(glActiveTexture(GL_TEXTURE0 + stage));
      TextureRef& t = m_textures[h.id];
      t.inst.Bind();
    }
  }

  void SetState(FlagType flags)
  {
    const uint32_t deltaFlags = flags ^ m_activeState;

    // depth test
    if (CheckAllMaskBits(deltaFlags, F_STATE_DEPTH_TEST))
    {
      gl::SwitchEnabledState(GL_DEPTH_TEST, CheckAllMaskBits(flags, F_STATE_DEPTH_TEST));
    }

    if (CheckAnyMaskBits(flags, F_STATE_DEPTH_COMPF_MASK) &&
      (flags & F_STATE_DEPTH_COMPF_MASK) != (m_activeState & F_STATE_DEPTH_COMPF_MASK))
    {
      GLenum depthFunc = GetDepthFunc(flags);
      XR_GL_CALL(glDepthFunc(depthFunc));
    }

    // depth write
    if (CheckAllMaskBits(deltaFlags, F_STATE_DEPTH_WRITE))
    {
      XR_GL_CALL(glDepthMask(CheckAllMaskBits(flags, F_STATE_DEPTH_WRITE)));
    }

    // blending
    if (CheckAllMaskBits(deltaFlags, F_STATE_ALPHA_BLEND))
    {
      bool alphaBlend = CheckAllMaskBits(flags, F_STATE_ALPHA_BLEND);
      gl::SwitchEnabledState(GL_BLEND, alphaBlend);
    }

    // blending function -- only change if it was set in the given state and is
    // different from the current one.
    if (CheckAnyMaskBits(flags, F_STATE_BLENDF_MASK) &&
      (flags & F_STATE_BLENDF_MASK) != (m_activeState & F_STATE_BLENDF_MASK))
    {
      GLenum  bfs[4];
      GetBlendFactors(flags, bfs);

      if (bfs[1] == kBlendFactors[0])  // default src alpha to src rgb.
      {
        bfs[1] = bfs[0];
      }

      if (bfs[3] == kBlendFactors[0])  // default dst alpha to dst rgb.
      {
        bfs[3] = bfs[2];
      }

      XR_GL_CALL(glBlendFuncSeparate(bfs[0], bfs[1], bfs[2], bfs[3]));
    }
    else
    {
      flags = (flags & ~F_STATE_BLENDF_MASK) | (m_activeState & F_STATE_BLENDF_MASK);
    }

    // culling
    if ((deltaFlags & (F_STATE_CULL_BACK | F_STATE_CULL_FRONT)) != 0)
    {
      uint32_t culling = flags & (F_STATE_CULL_BACK | F_STATE_CULL_FRONT);
      bool hasCulling = culling != 0;
      gl::SwitchEnabledState(GL_CULL_FACE, hasCulling);
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

    // wireframe
    if (CheckAllMaskBits(deltaFlags, F_STATE_WIREFRAME))
    {
      XR_GL_CALL(glPolygonMode(GL_FRONT_AND_BACK,
        CheckAllMaskBits(flags, F_STATE_WIREFRAME) ? GL_LINE : GL_FILL));
    }

    // stencil test
    if (CheckAllMaskBits(deltaFlags, F_STATE_STENCIL_TEST))
    {
      XR_GL_CALL(gl::SwitchEnabledState(GL_STENCIL_TEST,
        CheckAllMaskBits(flags, F_STATE_STENCIL_TEST)));
    }

    m_activeState = flags;
  }

  void SetStencilState(FlagType front, FlagType back)
  {
    if (back == F_STENCIL_SAME)
    {
      back = front;
    }

    const GLenum faces[] = { GL_FRONT, GL_BACK };
    int i = 0;
    for (auto state: { front, back })
    {
      const FlagType activeStencil = m_activeStencil[i];
      FlagType delta = state ^ activeStencil;
      if (CheckAnyMaskBits(delta, F_STENCIL_REF_MASK | F_STENCIL_MASK_MASK |
        F_STENCIL_COMPF_MASK))
      {
        // Try to fall back on previously set stencil function if no value given.
        if (!CheckAnyMaskBits(state, F_STENCIL_COMPF_MASK))
        {
          state |= activeStencil & F_STENCIL_COMPF_MASK;
        }

        GLenum stencilFunc = GetStencilFunc(state);
        XR_GL_CALL(glStencilFuncSeparate(faces[i], stencilFunc,
          state & F_STENCIL_REF_MASK,
          (state & F_STENCIL_MASK_MASK) >> F_STENCIL_MASK_SHIFT));
      }

      if (CheckAnyMaskBits(delta, F_STENCIL_OP_MASK))
      {
        // For those operations that weren't given, try to fall back on previously
        // set value.
        for (auto opMask: { F_STENCIL_FAIL_OP_MASK, F_STENCIL_DEPTH_FAIL_OP_MASK,
          F_STENCIL_ALL_PASS_OP_MASK })
        {
          if (!CheckAnyMaskBits(state, opMask))
          {
            state |= activeStencil & opMask;
          }
        }

        GLenum stencilOps[3];
        GetStencilOps(state, stencilOps);

        XR_GL_CALL(glStencilOpSeparate(faces[i], stencilOps[0], stencilOps[1],
          stencilOps[2]));
      }

      m_activeStencil[i] = state;
      ++i;
    }
  }

  void SetInstanceData(InstanceDataBufferHandle h, uint16_t offset, uint16_t count)
  {
    m_hActiveInstDataBuffer = h;
    if (h.IsValid())
    {
      VertexBufferObject& idbo = m_vbos[h.id];
      m_activeInstDataBufferStride = idbo.DecodeInstanceDataStride();
      m_instanceOffset = offset;
      m_instanceCount = count;
    }
  }

  void SetProgram(ProgramHandle h)
  {
    if (m_activeProgram != h)
    {
      Program& program = m_programs[h.id];
      XR_GL_CALL(glUseProgram(program.name));
      m_activeProgram = h;
    }
  }

  void SetFrameBuffer(FrameBufferHandle h)
  {
    if (m_activeFrameBuffer != h)
    {
      m_activeFrameBuffer = h;
      m_fbos[h.id].Bind();
    }
  }

  void Draw(VertexBufferHandle vbh, Primitive pt, uint32_t offset, uint32_t count)
  {
    XR_ASSERT(Gfx, m_activeProgram.id != ProgramHandle::INVALID_ID);
    Program& program = m_programs[m_activeProgram.id];
    XR_ASSERT(Gfx, program.linked == true);

    program.ResetAttribBindings();
    program.UpdateUniforms(m_uniforms.server.GetNumActive(), m_uniformData);

    VertexBufferObject const& vbo = m_vbos[vbh.id];
    VertexFormatRef const& vfr = m_vertexFormats[vbo.hFormat.id];
    vbo.Bind();
    program.BindAttributes(vfr.inst, 0);

    uint16_t instCount;
    ApplyInstanceData(program, instCount);

    program.UnbindUnusedAttributes();

    XR_GL_CALL(glDrawArraysInstanced(kPrimitiveTypes[uint8_t(pt)], offset, count,
      instCount));
  }

  void Draw(VertexBufferHandle vbh, IndexBufferHandle ibh, Primitive pt, uint32_t offset, uint32_t count)
  {
    XR_ASSERT(Gfx, m_activeProgram.id != ProgramHandle::INVALID_ID);
    Program& program = m_programs[m_activeProgram.id];
    XR_ASSERT(Gfx, program.linked == true);

    program.ResetAttribBindings();
    program.UpdateUniforms(m_uniforms.server.GetNumActive(), m_uniformData);

    VertexBufferObject const& vbo = m_vbos[vbh.id];
    VertexFormatRef const& vfr = m_vertexFormats[vbo.hFormat.id];
    vbo.Bind();
    program.BindAttributes(vfr.inst, 0);

    IndexBufferObject const& ibo = m_ibos[ibh.id];
    ibo.Bind();

    uint16_t instCount;
    ApplyInstanceData(program, instCount);

    program.UnbindUnusedAttributes();

    GLenum types[] = { GL_UNSIGNED_SHORT, GL_UNSIGNED_INT };
    XR_GL_CALL(glDrawElementsInstanced(kPrimitiveTypes[uint8_t(pt)], count,
      types[(ibo.flags & F_BUFFER_INDEX_32BITS) != 0],
      UINT_PTR_CAST(offset * ibo.indexSize), instCount));
  }

  void Flush()
  {
    XR_GL_CALL(glFlush());

    for (auto& cb : m_onFlush)
    {
      cb.Call(nullptr);
    }
  }

  void Present(bool resetState)
  {
    Flush();
    m_context->Swap();
  }

  void RegisterFlushCallback(Callback fn, void* userData)
  {
    m_onFlush.push_back({ fn, userData });
  }

  void RegisterShutdownCallback(Callback fn, void* userData)
  {
    m_onExit.push_back({ fn, userData });
  }

private:
  // data
  Context* const m_context; // no ownership

  ServicedArray<VertexFormatRef, 64> m_vertexFormats;
  std::unordered_map<uint32_t, VertexFormatHandle> m_vertexFormatHandles;

  ServicedArray<VertexBufferObject, 4096> m_vbos;
  ServicedArray<IndexBufferObject, 4096> m_ibos;
  ServicedArray<TextureRef, 1024, 3> m_textures;
  ServicedArray<FrameBuffer, 256, 1> m_fbos;

  ServicedArray<UniformRef, 1024> m_uniforms;
  void* m_uniformData[decltype(m_uniforms)::kSize];
  std::unordered_map<uint32_t, UniformHandle> m_uniformHandles;

  ServicedArray<ShaderRef, 512> m_shaders;
  ServicedArray<Program, 512> m_programs;

  GLuint m_vao = 0;
  FlagType m_activeState = F_STATE_NONE;
  FlagType m_activeStencil[2] = { };

  InstanceDataBufferHandle m_hActiveInstDataBuffer;
  uint16_t m_activeInstDataBufferStride = 0;
  uint16_t m_instanceOffset = 0;
  uint16_t m_instanceCount = 0;

  TextureHandle m_activeTextures[kMaxTextureStages];  // no ownership
  ProgramHandle m_activeProgram;  // no ownership
  FrameBufferHandle m_activeFrameBuffer;  // no ownership

  std::vector<CallbackObject> m_onFlush;
  std::vector<CallbackObject> m_onExit;

  // internal
  VertexBufferHandle CreateVertexBufferInternal(VertexFormatHandle hFormat, Buffer const& buffer, FlagType flags)
  {
    VertexBufferHandle h;
    h.id = uint16_t(m_vbos.server.Acquire());

    VertexBufferObject& vbo = m_vbos[h.id];
    XR_GL_CALL(glGenBuffers(1, &vbo.name));

    vbo.target = CheckAllMaskBits(flags, F_BUFFER_INDIRECT_DRAW) ?
      GL_DRAW_INDIRECT_BUFFER : GL_ARRAY_BUFFER;  // TODO: indirect draw support
    vbo.Bind();

    XR_GL_CALL(glBufferData(vbo.target, buffer.size, buffer.data,
      buffer.data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW));
    XR_GL_CALL(glBindBuffer(vbo.target, 0));

    vbo.hFormat = hFormat;
    vbo.flags = flags;

    return h;
  }

  void ApplyInstanceData(Program const& program, uint16_t& instCount)
  {
    bool isUsingInstanceData = m_hActiveInstDataBuffer.IsValid();
    instCount = isUsingInstanceData ? m_instanceCount : 1;
    if (isUsingInstanceData)
    {
      VertexBufferObject const& instBuffer = m_vbos[m_hActiveInstDataBuffer.id];
      instBuffer.Bind();
      program.BindInstanceData(m_activeInstDataBufferStride, m_instanceOffset);
    }
    else
    {
      program.UnbindInstanceData();
    }
  }

  void CreateDefaultTexture(TextureHandle const& handle, GLenum target)
  {
    auto& texture = m_textures.data[handle.id];
    texture.refCount = 0;
    texture.inst.name = 0;
    texture.inst.target = target;
  }
};

Impl* s_impl = nullptr;

} //

//=============================================================================
uint32_t VertexFormat::CalculateHash() const
{
  return Hash::Data32(m_attributes, sizeof(m_attributes) + sizeof(m_offset));
}

//=============================================================================
void Init(Context* context)
{
  XR_ASSERTMSG(Gfx, !s_impl, ("Already initialized."));
  s_impl = new Impl(context);
}

//=============================================================================
int GetLogicalWidth()
{
  return s_impl->GetLogicalWidth();
}

//=============================================================================
int GetLogicalHeight()
{
  return s_impl->GetLogicalHeight();
}

//=============================================================================
float GetLogicalAspectRatio()
{
  return GetLogicalWidth() / float(GetLogicalHeight());
}

//=============================================================================
int GetPhysicalWidth()
{
  return s_impl->GetPhysicalWidth();
}

//=============================================================================
int GetPhysicalHeight()
{
  return s_impl->GetPhysicalHeight();
}

//=============================================================================
float GetPhysicalAspectRatio()
{
  return GetPhysicalWidth() / float(GetPhysicalHeight());
}

//=============================================================================
void Shutdown()
{
  XR_ASSERTMSG(Gfx, s_impl, ("Not initialized."));
  delete s_impl;
  s_impl = nullptr;
}

//=============================================================================
VertexFormatHandle RegisterVertexFormat(VertexFormat const& format)
{
  return s_impl->RegisterVertexFormat(format);
}

//=============================================================================
void Release(VertexFormatHandle h)
{
  if (h.IsValid())
  {
    s_impl->Release(h);
  }
}

//=============================================================================
VertexBufferHandle CreateVertexBuffer(VertexFormatHandle hFormat, Buffer const& buffer, FlagType flags)
{
  return s_impl->CreateVertexBuffer(hFormat, buffer, flags);
}

//=============================================================================
void Release(VertexBufferHandle h)
{
  if (h.IsValid())
  {
    s_impl->Release(h);
  }
}

//=============================================================================
IndexBufferHandle CreateIndexBuffer(Buffer const& buffer, FlagType flags)
{
  return s_impl->CreateIndexBuffer(buffer, flags);
}

//=============================================================================
void Release(IndexBufferHandle h)
{
  if (h.IsValid())
  {
    s_impl->Release(h);
  }
}

//=============================================================================
InstanceDataBufferHandle CreateInstanceDataBuffer(Buffer const& buffer, uint16_t stride)
{
  return s_impl->CreateInstanceDataBuffer(buffer, stride);
}

//=============================================================================
void Release(InstanceDataBufferHandle h)
{
  if (h.IsValid())
  {
    s_impl->Release(h);
  }
}

//=============================================================================
TextureHandle CreateTexture(TextureFormat format, uint32_t width,
  uint32_t height, uint32_t depth, FlagType flags, Buffer const* buffer,
  size_t numBuffers)
{
  return s_impl->CreateTexture(format, width, height, depth, flags, buffer, numBuffers);
}

//=============================================================================
TextureHandle CreateTexture(TextureFormat format, uint32_t width, uint32_t height, uint32_t depth, FlagType flags)
{
  XR_ASSERTMSG(Gfx, !kTextureFormats[static_cast<int>(format)].compressed,
    ("Cannot create compressed texture without buffer."));
  Buffer buffer { 0, nullptr };
  return CreateTexture(format, width, height, depth, flags, &buffer, 1);
}

//=============================================================================
TextureHandle GetDefaultTexture2D()
{
  return kDefaultTexture2D;
}

//=============================================================================
TextureHandle GetDefaultTexture3D()
{
  return kDefaultTexture3D;
}

//=============================================================================
TextureHandle GetDefaultTextureCube()
{
  return kDefaultTextureCube;
}

//=============================================================================
TextureInfo GetTextureInfo(TextureHandle h)
{
  return s_impl->GetTextureInfo(h);
}

//=============================================================================
void Release(TextureHandle h)
{
  if (h.IsValid())
  {
    s_impl->Release(h);
  }
}

//=============================================================================
FrameBufferHandle GetDefaultFrameBuffer()
{
  return kDefaultFrameBuffer;
}

//=============================================================================
FrameBufferHandle CreateFrameBuffer(TextureFormat format, uint32_t width, uint32_t height,
  FlagType flags)
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
void ReadFrameBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
  TextureFormat format, void* mem)
{
  s_impl->ReadFrameBuffer(x, y, width, height, format, mem);
}

//=============================================================================
void ReadFrameBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
  TextureFormat format, uint16_t colorAttachment, void * mem)
{
  s_impl->ReadFrameBuffer(x, y, width, height, format, colorAttachment, mem);
}

//=============================================================================
void Release(FrameBufferHandle h)
{
  if (h.IsValid())
  {
    s_impl->Release(h);
  }
}

//=============================================================================
UniformHandle CreateUniform(char const* name, UniformType type, uint8_t arraySize)
{
  return s_impl->CreateUniform(name, type, arraySize);
}

//=============================================================================
void Release(UniformHandle h)
{
  if (h.IsValid())
  {
    s_impl->Release(h);
  }
}

//=============================================================================
ShaderHandle CreateShader(ShaderType t, Buffer const& buffer)
{
  return s_impl->CreateShader(t, buffer);
}

//=============================================================================
void Release(ShaderHandle h)
{
  if (h.IsValid())
  {
    s_impl->Release(h);
  }
}

//=============================================================================
ProgramHandle CreateProgram(ShaderHandle hVertex, ShaderHandle hFragment)
{
  return s_impl->CreateProgram(hVertex, hFragment);
}

//=============================================================================
void Release(ProgramHandle h)
{
  if (h.IsValid())
  {
    s_impl->Release(h);
  }
}

//=============================================================================
void Clear(FlagType flags, Color color, float depth, uint8_t stencil)
{
  s_impl->ClearBuffer(flags, color, depth, stencil);
}

//==============================================================================
void SetViewport(Rect const& rect)
{
  s_impl->SetViewport(rect);
}

//==============================================================================
void SetScissor(Rect const* rect)
{
  s_impl->SetScissor(rect);
}

//==============================================================================
void SetUniform(UniformHandle h, void const * data)
{
  s_impl->SetUniform(h, 1, data);
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
void SetState(FlagType flags)
{
  s_impl->SetState(flags);
}

//==============================================================================
void SetStencilState(FlagType front, FlagType back)
{
  s_impl->SetStencilState(front, back);
}

//==============================================================================
void SetInstanceData(InstanceDataBufferHandle h, uint16_t offset, uint16_t count)
{
  s_impl->SetInstanceData(h, offset, count);
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
void Draw(VertexBufferHandle vbh, Primitive primitive, uint32_t offset, uint32_t count)
{
  s_impl->Draw(vbh, primitive, offset, count);
}

//=============================================================================
void Draw(VertexBufferHandle vbh, IndexBufferHandle ibh, Primitive primitive,
  uint32_t offset, uint32_t count)
{
  s_impl->Draw(vbh, ibh, primitive, offset, count);
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

//==============================================================================
void RegisterFlushCallback(Callback fn, void* userData)
{
  s_impl->RegisterFlushCallback(fn, userData);
}

//==============================================================================
void RegisterShutdownCallback(Callback fn, void* userData)
{
  s_impl->RegisterShutdownCallback(fn, userData);
}

}
}
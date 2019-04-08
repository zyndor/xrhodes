//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "GfxResourceManager.hpp"
#include "GfxCore.hpp"
#include "xrgl.hpp"
#include "xr/math/Matrix.hpp"
#include "xr/Device.hpp"
#include "xr/math/SVector2.hpp"
#include "xr/events/SignalBroadcaster.hpp"
#include "xr/memory/Pool.hpp"
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
const GLint kInvalidLoc = -1;

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
  GL_POINTS,
  GL_LINES,
  GL_LINE_STRIP,
  GL_TRIANGLES,
  GL_TRIANGLE_STRIP,
  // TODO: remove along with Primitive::TriList / TriStrip.
  GL_TRIANGLES,
  GL_TRIANGLE_STRIP,
};

//=============================================================================
constexpr GLenum kBlendFactors[] =
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
constexpr GLenum kComparisons[] =
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
const GLenum kFilterModes[] =
{
  GL_LINEAR,
  GL_NEAREST,
  GL_LINEAR,
  GL_NEAREST,
  GL_LINEAR_MIPMAP_LINEAR,
  GL_NEAREST_MIPMAP_LINEAR,
  GL_LINEAR_MIPMAP_NEAREST,
  GL_NEAREST_MIPMAP_NEAREST,
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
struct TextureFormatGL
{
  GLenum internal;
  GLenum srgbInternal;
  GLenum format;
  GLenum type;
  // 2 bits for attachment type, then 6 bits for bit size each component.
  uint16_t attachmentTypeBits;
  bool compressed;
};

// Pack color / depth / stencil bits.
#define PACK_TEX_COMP(component, size) ((size) & 0x3f) << (2 + ((component) * 6))

const TextureFormatGL kTextureFormats[] =
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
// Implementation of resources.
//=============================================================================
inline
void BindVertexBuffer(VertexBufferObject const& vbo)
{
  XR_GL_CALL(glBindBuffer(vbo.target, vbo.name));
}

inline
void BindIndexBuffer(IndexBufferObject const& ibo)
{
  XR_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.name));
}

inline
void BindTexture(Texture const& texture)
{
  XR_GL_CALL(glBindTexture(texture.target, texture.name));
}

inline
void BindFrameBuffer(FrameBufferObject const& fbo)
{
  XR_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, fbo.name));
}

void UpdateProgramUniforms(Program& program, size_t numUniforms, void const* const* uniformData)
{
  if (auto uniforms = program.uniforms)
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

void ReleaseProgramUniforms(Program& program, ResourceManager* resources)
{
  auto uniformBuffer = program.uniforms;
  uniformBuffer->Reset();

  UniformType ignoreType;
  uint8_t ignoreSize;
  uint16_t ignoreLoc;
  UniformHandle hUniform;
  GLint ignoreGlLoc;

  XR_DEBUG_ONLY(size_t numActiveUniforms = resources->GetUniforms().server.
    GetNumActive());
  while (!uniformBuffer->AtEnd())
  {
    uniformBuffer->ReadHandle(ignoreType, ignoreSize, ignoreLoc, hUniform);
    XR_ASSERT(Gfx, hUniform.id < numActiveUniforms);
    resources->Release(hUniform);
    uniformBuffer->Read(ignoreGlLoc);
  }

  uniformBuffer->Finalize();

  ConstBuffer::Destroy(program.uniforms);
}

void ResetProgramAttribBindings(Program& program)
{
  auto& unbound = program.unboundAttribs;
  static_assert(sizeof(program.activeAttribs) == sizeof(unbound),
    "Size of active / unbound attribs array must match.");
  std::memcpy(unbound, program.activeAttribs, sizeof(unbound));
}

void BindProgramAttributes(Program& program, VertexFormat const& format, uint32_t baseVertex)
{
  const uint32_t stride = format.GetStride();
  uint8_t numComponents;
  bool normalized;
  uint16_t offset;
  for (int i = 0; i < program.numActiveAttribs; ++i)
  {
    uint8_t attrib = program.activeAttribs[i];
    GLint loc = static_cast<GLint>(program.attribLoc[attrib]);

    if (loc != kInvalidLoc && format.Get(Attribute(attrib), numComponents, normalized, offset))
    {
      XR_GL_CALL(glEnableVertexAttribArray(loc));
      XR_GL_CALL(glVertexAttribDivisor(loc, 0));

      uint32_t attribBase = baseVertex * stride + offset;;
      XR_GL_CALL(glVertexAttribPointer(loc, numComponents, GL_FLOAT, normalized, format.GetStride(), UINT_PTR_CAST(attribBase)));

      program.unboundAttribs[i] = uint8_t(Attribute::kCount);
    }
  }
}

void UnbindProgramUnusedAttributes(Program const& program)
{
  for (int i = 0; i < program.numActiveAttribs; ++i)
  {
    if (program.unboundAttribs[i] != uint8_t(Attribute::kCount))
    {
      uint8_t attrib = program.activeAttribs[i];
      GLint loc = static_cast<GLint>(program.attribLoc[attrib]);

      XR_GL_CALL(glDisableVertexAttribArray(loc));
    }
  }
}

void BindProgramInstanceData(Program const& program, uint16_t stride, uint32_t baseInstance)
{
  uint32_t offset = baseInstance * stride;
  for (int i = 0; static_cast<GLint>(program.instanceDataLoc[i]) != kInvalidLoc;
    ++i)
  {
    GLint loc = static_cast<GLint>(program.instanceDataLoc[i]);
    XR_GL_CALL(glEnableVertexAttribArray(loc));
    XR_GL_CALL(glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, stride,
      UINT_PTR_CAST(offset)));
    XR_GL_CALL(glVertexAttribDivisor(loc, 1));
    offset += 4 * sizeof(float);
  }
}

void UnbindProgramInstanceData(Program const& program)
{
  for (int i = 0; static_cast<GLint>(program.instanceDataLoc[i]) != kInvalidLoc; ++i)
  {
    GLint loc = static_cast<GLint>(program.instanceDataLoc[i]);
    XR_GL_CALL(glDisableVertexAttribArray(loc));
  }
}

//=============================================================================
// Implementation of Core - statics
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

void InitExtensions(int minorMajorVersion)
{
  auto bufferCmp = [](Buffer const& lhs, Buffer const& rhs) {
    return strncmp((char const*)lhs.data, (char const*)rhs.data,
      std::max(lhs.size, rhs.size)) < 0;
  };
  std::set<Buffer, decltype(bufferCmp)> extensions(bufferCmp);

  // get extensions
  char const* ext = (char const*)glGetString(GL_EXTENSIONS);
  XR_GL_IGNORE_ERROR;
  if (ext)
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

//=============================================================================
// Implementation of Core - members
//=============================================================================
struct CoreContext
{
  Context* mContext;
  ResourceManager* mResources;

  GLuint mVao = 0;
  FlagType mActiveState = F_STATE_NONE;
  FlagType mActiveStencil[2] = {};

  InstanceDataBufferHandle mActiveInstDataBuffer;
  uint16_t mActiveInstDataBufferStride = 0;
  uint32_t mInstanceOffset = 0;
  uint32_t mInstanceCount = 0;

  TextureHandle mActiveTextures[kMaxTextureStages];  // no ownership
  ProgramHandle mActiveProgram;  // no ownership
  FrameBufferHandle mActiveFrameBuffer;  // no ownership

  // NOTE: we're just providing these, never actually use them in here;
  // they should be synchronized, and one major version later maybe they will be.
  SignalBroadcaster<> mOnFlush;
  SignalBroadcaster<> mOnShutdown;
}* sContext = nullptr;

void CreateVertexBufferInternal(VertexFormatHandle hFormat, Buffer const& buffer,
  FlagType flags, VertexBufferObject& vbo)
{
  XR_GL_CALL(glGenBuffers(1, &vbo.name));

  vbo.target = CheckAllMaskBits(flags, F_BUFFER_INDIRECT_DRAW) ?
    GL_DRAW_INDIRECT_BUFFER : GL_ARRAY_BUFFER;  // TODO: indirect draw support
  BindVertexBuffer(vbo);

  XR_GL_CALL(glBufferData(vbo.target, buffer.size, buffer.data,
    buffer.data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW));
  XR_GL_CALL(glBindBuffer(vbo.target, 0));

  vbo.hFormat = hFormat;
  vbo.flags = flags;
}

void ApplyInstanceData(Program const& program, uint32_t& instCount)
{
  bool isUsingInstanceData = sContext->mActiveInstDataBuffer.IsValid();
  instCount = isUsingInstanceData ? sContext->mInstanceCount : 1;
  if (isUsingInstanceData)
  {
    auto& vbos = sContext->mResources->GetVbos();
    VertexBufferObject const& instBuffer = vbos[sContext->mActiveInstDataBuffer.id];
    BindVertexBuffer(instBuffer);
    BindProgramInstanceData(program, sContext->mActiveInstDataBufferStride,
      sContext->mInstanceOffset);
    sContext->mActiveInstDataBuffer.Invalidate();
  }
  else
  {
    UnbindProgramInstanceData(program);
  }
}

void CreateDefaultTexture(TextureHandle const& handle, GLenum target)
{
  auto& textures = sContext->mResources->GetTextures();
  auto& texture = textures[handle.id];
  texture.refCount = 0;
  texture.inst.name = 0;
  texture.inst.target = target;
}

} // nonamespace

//=============================================================================
void Core::Init(Context* context, ResourceManager* resources)
{
  XR_TRACE(Gfx, ("Initialising..."));
  sContext = new CoreContext;
  sContext->mContext = context;
  context->Init();

  sContext->mResources = resources;
  XR_ASSERT(Gfx, sContext->mResources);

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

  sContext->mActiveState = (sContext->mActiveState & ~F_STATE_DEPTH_COMPF_MASK) |
    DepthFuncToState(Comparison::LEQUAL);
  XR_GL_CALL(glDepthFunc(GL_LEQUAL));

  sContext->mActiveState = (sContext->mActiveState & ~F_STATE_BLENDF_MASK) |
    BlendFactorToState(BlendFactor::SRC_ALPHA, BlendFactor::INV_SRC_ALPHA);
  XR_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  const FlagType activeStencil = StencilToState(0x00, 0xff, Comparison::FAIL,
    StencilOp::KEEP, StencilOp::KEEP, StencilOp::KEEP);
  sContext->mActiveStencil[0] = activeStencil;
  sContext->mActiveStencil[1] = activeStencil;
  XR_GL_CALL(glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_NEVER, 0x00, 0xff));
  XR_GL_CALL(glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP));

  if (context->GetGlVersionMajor() >= 3 ||
    s_extensions[ExtensionGL::OES_vertex_array_object].supported ||
    s_extensions[ExtensionGL::ARB_vertex_array_object].supported)
  {
    XR_GL_CALL(glGenVertexArrays(1, &sContext->mVao));
    XR_GL_CALL(glBindVertexArray(sContext->mVao));
  }

  // create default textures and framebuffer
  CreateDefaultTexture(GetDefaultTexture2D(), GL_TEXTURE_2D);
  CreateDefaultTexture(GetDefaultTexture3D(), GL_TEXTURE_3D);
  CreateDefaultTexture(GetDefaultTextureCube(), GL_TEXTURE_CUBE_MAP);

  auto& fbos = sContext->mResources->GetFbos();
  auto& defaultFbo = fbos[GetDefaultFrameBuffer().id];
  defaultFbo.numColorAttachments = 2;
  defaultFbo.name = 0;

  sContext->mActiveFrameBuffer = GetDefaultFrameBuffer();

  XR_TRACE(Gfx, ("Initialisation complete."));
}

void Core::Release(VertexFormatHandle h)
{
  sContext->mResources->Release(h);
}

void Core::CreateVertexBuffer(VertexFormatHandle hFormat, Buffer const& buffer,
  FlagType flags, VertexBufferObject& vbo)
{
  XR_ASSERT(Gfx, hFormat.IsValid());
  auto& vforms = sContext->mResources->GetVertexFormats();
  ++(vforms[hFormat.id].refCount);

  XR_TRACEIF(Gfx, CheckAllMaskBits(flags, F_BUFFER_INSTANCE_DATA),
    ("Ignored instance data buffer request in CreateVertexBuffer()."));
  CreateVertexBufferInternal(hFormat, buffer, flags & ~F_BUFFER_INSTANCE_DATA, vbo);
}

void Core::Release(VertexBufferHandle h)
{
  auto& vbos = sContext->mResources->GetVbos();
  VertexBufferObject& vbo = vbos[h.id];
  XR_GL_CALL(glBindBuffer(vbo.target, 0));
  XR_GL_CALL(glDeleteBuffers(1, &vbo.name));

  if (vbo.hFormat.IsValid())  // instance data buffers don't have this.
  {
    sContext->mResources->Release(vbo.hFormat);
  }

  std::memset(&vbo, 0x00, sizeof(vbo));
  vbos.server.Release(h.id);
}

void Core::CreateIndexBuffer(Buffer const& buffer, FlagType flags, IndexBufferObject& ibo)
{
  XR_GL_CALL(glGenBuffers(1, &ibo.name));
  BindIndexBuffer(ibo);
  XR_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.size, buffer.data,
    buffer.data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW));

  ibo.indexSize = CheckAllMaskBits(flags, F_BUFFER_INDEX_32BITS) ? sizeof(uint32_t) : sizeof(uint16_t);
  ibo.flags = flags;
}

void Core::Release(IndexBufferHandle h)
{
  auto& ibos = sContext->mResources->GetIbos();
  IndexBufferObject& ibo = ibos[h.id];
  XR_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  XR_GL_CALL(glDeleteBuffers(1, &ibo.name));

  std::memset(&ibo, 0x00, sizeof(ibo));
  ibos.server.Release(h.id);
}

void Core::CreateInstanceDataBuffer(Buffer const& buffer, uint16_t stride,
  VertexBufferObject& idbo)
{
  XR_ASSERT(Gfx, buffer.data);
  XR_ASSERTMSG(Gfx, (stride & 0xf) == 0,
    ("16x bytes required for stride, got: %d", stride));

  // Invalid vertexformat (and GL_ARRAY_BUFFER target) signify a vbo used
  // for instance data.

  // Encode stride in flags.
  FlagType flags = VertexBufferObject::EncodeInstanceDataStride(stride);

  CreateVertexBufferInternal(VertexFormatHandle(), buffer, flags, idbo);
}

void Core::Release(InstanceDataBufferHandle h)
{
  VertexBufferHandle hConv;
  hConv.id = h.id;
  Core::Release(hConv);
}

void Core::CreateTexture(Buffer const* buffers, uint8_t numBuffers, TextureRef& tr)
{
  auto& t = tr.inst;
  XR_ASSERT(Gfx, numBuffers > 0);

  XR_GL_CALL(glGenTextures(1, &t.name));
  const auto format = t.info.format;
  auto width = t.info.width;
  auto height = t.info.height;
  const auto depth = t.info.depth;
  const auto flags = t.info.flags;

  // determine target
  t.target = CheckAllMaskBits(flags, F_TEXTURE_CUBE) ? GL_TEXTURE_CUBE_MAP :
    (depth > 0 ? GL_TEXTURE_3D : GL_TEXTURE_2D);
  XR_ASSERT(Gfx, t.target != GL_TEXTURE_CUBE_MAP || (numBuffers % 6) == 0);

  // bind it
  BindTexture(t);

  GLenum  wrapMode(CheckAllMaskBits(flags, F_TEXTURE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
  XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_WRAP_S, wrapMode));
  XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_WRAP_T, wrapMode));
  if (t.target == GL_TEXTURE_3D || t.target == GL_TEXTURE_CUBE_MAP)
  {
    XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_WRAP_R, wrapMode));
  }

  int fm = (CheckAllMaskBits(flags, F_TEXTURE_FILTER_POINT) ? 1 : 0) |
    ((CheckAllMaskBits(flags, F_TEXTURE_FILTER_POINT) ? 1 : 0) << 1);
  GLenum minFilter = kFilterModes[fm + ((t.info.mipLevels > 1) * 4)];
  XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_MIN_FILTER, minFilter));

  GLenum magFilter = kFilterModes[fm];
  XR_GL_CALL(glTexParameteri(t.target, GL_TEXTURE_MAG_FILTER, magFilter));

  auto& formatDesc = kTextureFormats[static_cast<int>(format)];
  GLenum fmtInternal = (CheckAllMaskBits(flags, F_TEXTURE_SRGB) && formatDesc.srgbInternal != GL_ZERO) ?
    formatDesc.srgbInternal : formatDesc.internal;

  GLenum target = t.target != GL_TEXTURE_CUBE_MAP ? t.target : GL_TEXTURE_CUBE_MAP_POSITIVE_X;
  const uint8_t sides = t.target != GL_TEXTURE_CUBE_MAP ? 1 : 6;
  const uint8_t layers = numBuffers / sides;
  XR_ASSERT(Gfx, layers > 0);
  int iBuffer = 0;
  for (uint8_t i = 0; i < layers; ++i)
  {
    for (uint8_t j = 0; j < sides; ++j)
    {
      if (formatDesc.compressed)
      {
        switch (t.target)
        {
        case GL_TEXTURE_3D:
          XR_GL_CALL(glCompressedTexImage3D(target, i, fmtInternal, width, height,
            depth, 0, static_cast<GLsizei>(buffers[iBuffer].size), buffers[iBuffer].data));
          break;

        case GL_TEXTURE_2D:
        case GL_TEXTURE_CUBE_MAP:
          XR_GL_CALL(glCompressedTexImage2D(target + j, i, fmtInternal,
            width, height, 0, static_cast<GLsizei>(buffers[iBuffer].size),
            buffers[iBuffer].data));
          break;
        }
      }
      else
      {
        switch (t.target)
        {
        case GL_TEXTURE_3D:
          XR_GL_CALL(glTexImage3D(t.target, i, fmtInternal, width, height, depth,
            0, formatDesc.format, formatDesc.type, buffers[iBuffer].data));
          break;

        case GL_TEXTURE_2D:
        case GL_TEXTURE_CUBE_MAP:
          XR_GL_CALL(glTexImage2D(target + j, i, fmtInternal,
            width, height, 0, formatDesc.format, formatDesc.type, buffers[iBuffer].data));
          break;
        }
      }

      ++iBuffer;
    }

    width /= 2;
    height /= 2;
  }

  if (layers < t.info.mipLevels)
  {
    XR_GL_CALL(glGenerateMipmap(t.target));
  }

  tr.refCount = 1;
}

void Core::Release(TextureHandle h)
{
  auto& textures = sContext->mResources->GetTextures();
  TextureRef& texture = textures[h.id];
  XR_ASSERT(Gfx, texture.inst.name != 0); // trying to release a default texture
  XR_ASSERT(Gfx, texture.refCount > 0);
  --texture.refCount;
  if (texture.refCount == 0)
  {
    XR_GL_CALL(glDeleteTextures(1, &texture.inst.name));
    std::memset(&texture.inst, 0x00, sizeof(Texture));

    textures.server.Release(h.id);
  }
}

bool Core::CreateFrameBuffer(TextureFormat format, uint16_t width, uint16_t height,
  FlagType flags, FrameBufferObject& fbo)
{
  TextureHandle h = Gfx::CreateTexture(format, width, height, 0, flags);
  return Core::CreateFrameBuffer(1, &h, true, fbo);
}

bool Core::CreateFrameBuffer(uint8_t textureCount, TextureHandle const* hTextures,
  bool ownTextures, FrameBufferObject& fbo)
{
  std::vector<FrameBufferAttachment>  attachments(textureCount);
  std::generate(attachments.begin(), attachments.end(), [&hTextures]() {
    FrameBufferAttachment a = { *hTextures, 0, 0 };
    ++hTextures;
    return a;
  });
  return Core::CreateFrameBuffer(textureCount, attachments.data(), ownTextures, fbo);
}

bool Core::CreateFrameBuffer(uint8_t textureCount, FrameBufferAttachment const* attachments,
  bool ownTextures, FrameBufferObject& fbo)
{
  XR_ASSERT(Gfx, textureCount < XR_ARRAY_SIZE(FrameBufferObject::hTextures));

  XR_GL_CALL(glGenFramebuffers(1, &fbo.name));
  BindFrameBuffer(fbo);

  fbo.numTextures = textureCount;
  GLenum colorAttachments[XR_ARRAY_SIZE(FrameBufferObject::hTextures)];
  int numColorAttachments = 0;
  auto& textures = sContext->mResources->GetTextures();
  for (uint8_t i = 0; i < textureCount; ++i)
  {
    FrameBufferAttachment const& att = attachments[i];
    fbo.hTextures[i] = att.hTexture;

    TextureRef& texture = textures[att.hTexture.id];
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

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  bool result = GL_FRAMEBUFFER_COMPLETE == status;
  if (!result)
  {
    XR_TRACE(Gfx, ("Failed to create framebuffer, status: 0x%x", status));

    if (!ownTextures)
    {
      for (uint8_t i = 0; i < textureCount; ++i)
      {
        Core::Release(attachments[i].hTexture);
      }
    }
  }
  return result;
}

void Core::Release(FrameBufferHandle h)
{
  auto& fbos = sContext->mResources->GetFbos();
  FrameBufferObject& fbo = fbos[h.id];
  BindFrameBuffer(fbo);

  for (uint8_t i = 0; i < fbo.numTextures; ++i)
  {
    Core::Release(fbo.hTextures[i]);
  }
  std::memset(&fbo, 0x00, sizeof(FrameBufferObject));

  XR_GL_CALL(glDeleteFramebuffers(1, &fbo.name));
  fbos.server.Release(h.id);
}

bool Core::CreateShader(ShaderType type, Buffer const& buffer, ShaderRef& sr)
{
  Shader& shader = sr.inst;

  GLenum typegl = kShaderTypes[size_t(type)];
  XR_GL_CALL(shader.name = glCreateShader(typegl));

  auto len = static_cast<GLint>(buffer.size);
  XR_ASSERT(Gfx, len <= std::numeric_limits<GLint>::max());
  XR_GL_CALL(glShaderSource(shader.name, 1,
    reinterpret_cast<GLchar const* const*>(&buffer.data), &len));
  XR_GL_CALL(glCompileShader(shader.name));

  GLint params = -1;
  XR_GL_CALL(glGetShaderiv(shader.name, GL_COMPILE_STATUS, &params));
  bool success = params == GL_TRUE;
  if (success)
  {
    sr.refCount = 1;
  }
  else
  {
    char log[1024];
    XR_GL_CALL(glGetShaderInfoLog(shader.name, sizeof(log), 0, log));
    XR_TRACE(Gfx, ("\n%s", buffer.data));
    XR_TRACE(Gfx, ("Failed to compile shader: %s", log));

    XR_GL_CALL(glDeleteShader(shader.name));
  }
  return success;
}

void Core::Release(ShaderHandle h)
{
  auto& shaders = sContext->mResources->GetShaders();
  ShaderRef& sr = shaders[h.id];
  XR_ASSERT(Gfx, sr.refCount > 0);
  --sr.refCount;
  if (sr.refCount == 0)
  {
    XR_GL_CALL(glDeleteShader(sr.inst.name));
    std::memset(&sr.inst, 0x00, sizeof(Shader));

    shaders.server.Release(h.id);
  }
}

bool Core::CreateProgram(ShaderHandle hVertex, ShaderHandle hFragment, Program& program)
{
  // Create program
  XR_GL_CALL(program.name = glCreateProgram());

  auto& shaders = sContext->mResources->GetShaders();
  ShaderRef& shdVert = shaders[hVertex.id];
  ShaderRef& shdFrag = shaders[hFragment.id];

  // attempt to attach shaders
  GLint attachResult;
  XR_GL_CALL_RESULT(glAttachShader(program.name, shdVert.inst.name), attachResult);
  bool success = attachResult == GL_NO_ERROR;
  if (success)
  {
    program.hVertex = hVertex;
    XR_GL_CALL_RESULT(glAttachShader(program.name, shdFrag.inst.name), attachResult);
    success = attachResult == GL_NO_ERROR;
  }

  if (success)
  {
    // attempt to link program
    program.hFragment = hFragment;
    XR_GL_CALL(glLinkProgram(program.name));

    GLint params = -1;
    XR_GL_CALL(glGetProgramiv(program.name, GL_LINK_STATUS, &params));
    success = params == GL_TRUE;
    program.linked = success;
    if (!success)
    {
      char log[1024];
      XR_GL_CALL(glGetProgramInfoLog(program.name, sizeof(log), 0, log));
      XR_TRACE(Gfx, ("Failed to link program (hVertex: %d, hFragment: %d): %s",
        hVertex, hFragment, log));
    }
  }

  if (success)
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

    std::memset(reinterpret_cast<GLint*>(program.attribLoc), 0xff, sizeof(program.attribLoc));
    std::fill(program.activeAttribs, program.activeAttribs +
      XR_ARRAY_SIZE(program.activeAttribs), uint8_t(Attribute::kCount));
    ResetProgramAttribBindings(program);
    //XR_GL_CALL(glGetProgramiv(program.name, GL_ACTIVE_ATTRIBUTES, &program.numActiveAttribs));
    for (int i = 0; i < uint8_t(Attribute::kCount); ++i)
    {
      XR_GL_CALL(GLint loc = glGetAttribLocation(program.name,
        Const::kAttributeName[i]));
      if (loc != kInvalidLoc)
      {
        program.attribLoc[i] = loc;
        program.activeAttribs[program.numActiveAttribs] = i;
        XR_TRACE(Gfx, ("Attribute %s at location %d",
          Const::kAttributeName[i], loc));

        ++program.numActiveAttribs;
      }
    }

    size_t used = 0;
    for (uint32_t i = 0; i < kMaxInstanceData; ++i)
    {
      XR_GL_CALL(GLint loc = glGetAttribLocation(program.name,
        Const::kInstanceDataName[i]));
      if (loc != kInvalidLoc)
      {
        program.instanceDataLoc[used] = loc;
        XR_TRACE(Gfx, ("Attribute %s at location %d",
          Const::kInstanceDataName[i], loc));

        ++used;
      }
    }
    program.instanceDataLoc[used] = kInvalidLoc;

    // process uniforms
    GLint num, loc;
    XR_GL_CALL(glGetProgramiv(program.name, GL_ACTIVE_UNIFORMS, &num));
    for (int i = 0; i < num; ++i)
    {
      XR_GL_CALL(glGetActiveUniform(program.name, i, maxLen, NULL, &arraySize, &type, nameBuffer));
      loc = glGetUniformLocation(program.name, nameBuffer);

      auto hUniform = sContext->mResources->FindUniform(nameBuffer);
      if (hUniform.IsValid())
      {
        auto& uniforms = sContext->mResources->GetUniforms();
        UniformRef& ur = uniforms[hUniform.id];
        ++ur.refCount;

        if (!program.uniforms)
        {
          program.uniforms = ConstBuffer::Create(1024);
        }
        program.uniforms->WriteHandle(ur.inst.type, ur.inst.arraySize, 0, hUniform);
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
  if (success)
  {
    if (program.uniforms)
    {
      program.uniforms->Finalize();
    }

    ++shdFrag.refCount;
    ++shdVert.refCount;
  }
  else
  {
    // failure - delete program
    glDetachShader(program.name, shdVert.inst.name);
    glDetachShader(program.name, shdFrag.inst.name);
    XR_GL_IGNORE_ERROR;

    if(program.uniforms)
    {
      ReleaseProgramUniforms(program, sContext->mResources);
    }

    XR_GL_CALL(glDeleteProgram(program.name));
    program.name = 0;
  }

  return success;
}

void Core::Release(ProgramHandle h)
{
  auto& programs = sContext->mResources->GetPrograms();
  Program& p = programs[h.id];
  XR_GL_CALL(glUseProgram(0));
  XR_GL_CALL(glDeleteProgram(p.name));
  p.name = 0;
  p.linked = false;

  if (p.hVertex.IsValid())
  {
    Core::Release(p.hVertex);
    p.hVertex.Invalidate();
  }

  if (p.hFragment.IsValid())
  {
    Core::Release(p.hFragment);
    p.hFragment.Invalidate();
  }

  if (p.uniforms)
  {
    ReleaseProgramUniforms(p, sContext->mResources);
  }
  p.uniforms = nullptr;

  programs.server.Release(h.id);
}

void Core::Clear(FlagType flags, Color const& color, float depth, uint8_t stencil)
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
    tempDepthWrite = !CheckAllMaskBits(sContext->mActiveState, F_STATE_DEPTH_WRITE);
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

void Core::SetViewport(Rect const& rect)
{
  XR_GL_CALL(glViewport(rect.x, rect.y, rect.w, rect.h));
}

void Core::SetScissor(Rect const* rect)
{
  bool state = rect != nullptr;
  gl::SwitchEnabledState(GL_SCISSOR_TEST, state);
  if (state)
  {
    XR_GL_CALL(glScissor(rect->x, rect->y, rect->w, rect->h));
  }
}

void Core::SetUniform(UniformHandle h, Buffer const& buffer)
{
  auto uniformData = sContext->mResources->GetUniformData();
  std::memcpy(uniformData[h.id], buffer.data, buffer.size);
}

void Core::SetTexture(TextureHandle h, uint8_t stage)
{
  if (sContext->mActiveTextures[stage] != h)
  {
    sContext->mActiveTextures[stage] = h;
    XR_GL_CALL(glActiveTexture(GL_TEXTURE0 + stage));

    auto& textures = sContext->mResources->GetTextures();
    TextureRef& t = textures[h.id];
    BindTexture(t.inst);
  }
}

void Core::SetState(FlagType flags)
{
  const uint32_t deltaFlags = flags ^ sContext->mActiveState;

  // depth test
  if (CheckAllMaskBits(deltaFlags, F_STATE_DEPTH_TEST))
  {
    gl::SwitchEnabledState(GL_DEPTH_TEST, CheckAllMaskBits(flags, F_STATE_DEPTH_TEST));
  }

  if (CheckAnyMaskBits(flags, F_STATE_DEPTH_COMPF_MASK) &&
    (flags & F_STATE_DEPTH_COMPF_MASK) != (sContext->mActiveState & F_STATE_DEPTH_COMPF_MASK))
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
    (flags & F_STATE_BLENDF_MASK) != (sContext->mActiveState & F_STATE_BLENDF_MASK))
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
    flags = (flags & ~F_STATE_BLENDF_MASK) | (sContext->mActiveState & F_STATE_BLENDF_MASK);
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

  sContext->mActiveState = flags;
}

void Core::SetStencilState(FlagType front, FlagType back)
{
  if (back == F_STENCIL_SAME)
  {
    back = front;
  }

  const GLenum faces[] = { GL_FRONT, GL_BACK };
  int i = 0;
  for (auto state: { front, back })
  {
    const FlagType activeStencil = sContext->mActiveStencil[i];
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

    sContext->mActiveStencil[i] = state;
    ++i;
  }
}

void Core::SetInstanceData(InstanceDataBufferHandle h, uint32_t offset, uint32_t count)
{
  sContext->mActiveInstDataBuffer = h;
  if (h.IsValid())
  {
    auto& vbos = sContext->mResources->GetVbos();
    VertexBufferObject& idbo = vbos[h.id];
    sContext->mActiveInstDataBufferStride = idbo.DecodeInstanceDataStride();
    sContext->mInstanceOffset = offset;
    sContext->mInstanceCount = count;
  }
}

void Core::SetProgram(ProgramHandle h)
{
  if (sContext->mActiveProgram != h)
  {
    auto& programs = sContext->mResources->GetPrograms();
    Program& program = programs[h.id];
    XR_GL_CALL(glUseProgram(program.name));
    sContext->mActiveProgram = h;
  }
}

void Core::SetFrameBuffer(FrameBufferHandle h)
{
  if (sContext->mActiveFrameBuffer != h)
  {
    sContext->mActiveFrameBuffer = h;

    auto& fbos = sContext->mResources->GetFbos();
    BindFrameBuffer(fbos[h.id]);
  }
}

void Core::Draw(VertexBufferHandle vbh, Primitive pt, uint32_t offset, uint32_t count)
{
  XR_ASSERT(Gfx, sContext->mActiveProgram.IsValid());
  auto& programs = sContext->mResources->GetPrograms();
  Program& program = programs[sContext->mActiveProgram.id];
  XR_ASSERT(Gfx, program.linked == true);

  ResetProgramAttribBindings(program);
  auto& uniforms = sContext->mResources->GetUniforms();
  auto uniformData = sContext->mResources->GetUniformData();
  UpdateProgramUniforms(program, uniforms.server.GetNumActive(), uniformData);

  auto& vbos = sContext->mResources->GetVbos();
  VertexBufferObject const& vbo = vbos[vbh.id];

  auto& vforms = sContext->mResources->GetVertexFormats();
  VertexFormatRef const& vfr = vforms[vbo.hFormat.id];
  BindVertexBuffer(vbo);
  BindProgramAttributes(program, vfr.inst, 0);

  uint32_t instCount;
  ApplyInstanceData(program, instCount);

  UnbindProgramUnusedAttributes(program);

  XR_GL_CALL(glDrawArraysInstanced(kPrimitiveTypes[uint8_t(pt)], offset, count,
    instCount));
}

void Core::Draw(VertexBufferHandle vbh, IndexBufferHandle ibh, Primitive pt,
  uint32_t offset, uint32_t count)
{
  XR_ASSERT(Gfx, sContext->mActiveProgram.id != ProgramHandle::INVALID_ID);
  auto& programs = sContext->mResources->GetPrograms();
  Program& program = programs[sContext->mActiveProgram.id];
  XR_ASSERT(Gfx, program.linked == true);

  ResetProgramAttribBindings(program);

  auto& uniforms = sContext->mResources->GetUniforms();
  auto uniformData = sContext->mResources->GetUniformData();
  UpdateProgramUniforms(program, uniforms.server.GetNumActive(), uniformData);

  auto& vbos = sContext->mResources->GetVbos();
  VertexBufferObject const& vbo = vbos[vbh.id];

  auto& vforms = sContext->mResources->GetVertexFormats();
  VertexFormatRef const& vfr = vforms[vbo.hFormat.id];
  BindVertexBuffer(vbo);
  BindProgramAttributes(program, vfr.inst, 0);

  auto& ibos = sContext->mResources->GetIbos();
  IndexBufferObject const& ibo = ibos[ibh.id];
  BindIndexBuffer(ibo);

  uint32_t instCount;
  ApplyInstanceData(program, instCount);

  UnbindProgramUnusedAttributes(program);

  GLenum types[] = { GL_UNSIGNED_SHORT, GL_UNSIGNED_INT };
  XR_GL_CALL(glDrawElementsInstanced(kPrimitiveTypes[uint8_t(pt)], count,
    types[(ibo.flags & F_BUFFER_INDEX_32BITS) != 0],
    UINT_PTR_CAST(offset * ibo.indexSize), instCount));
}

void Core::Flush()
{
  XR_GL_CALL(glFlush());
}

void Core::Present(bool resetState)
{
  Flush();
  sContext->mContext->Swap();
}

void  Core::ReadFrameBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
  TextureFormat format, uint8_t colorAttachment, void* mem,
  ReadFrameBufferCompleteCallback* onComplete)
{
  auto hFbo = sContext->mActiveFrameBuffer;
  XR_ASSERT(Gfx, hFbo.IsValid());
  XR_ASSERT(Gfx, colorAttachment < sContext->mResources->
    GetFbos()[sContext->mActiveFrameBuffer.id].numColorAttachments);
  TextureFormatGL const& texFormat = kTextureFormats[int(format)];
  GLenum formatGl = texFormat.format;
  GLenum type = texFormat.type;
  auto attachment = (hFbo == GetDefaultFrameBuffer() ?
    GL_FRONT : GL_COLOR_ATTACHMENT0) + colorAttachment;
  XR_GL_CALL(glReadBuffer(attachment));
  XR_GL_CALL(glReadPixels(x, y, width, height, formatGl, type, mem));

  if (onComplete)
  {
    onComplete->Call(mem);
  }
}

Signal<void>& Core::FlushSignal()
{
  return sContext->mOnFlush;
}

void Core::OnFlush()
{
  sContext->mOnFlush.Broadcast();
}

Signal<void>& Core::ShutdownSignal()
{
  return sContext->mOnShutdown;
}

void Core::OnShutdown()
{
  sContext->mOnShutdown.Broadcast();
}

void Core::Shutdown()
{
  XR_TRACE(Gfx, ("Shutting down."));
  if (sContext->mVao != 0)
  {
    XR_GL_CALL(glDeleteVertexArrays(1, &sContext->mVao));
  }

  sContext->mContext->Shutdown();

  delete sContext;
  sContext =nullptr;

  XR_TRACE(Gfx, ("Shutdown complete."));
}

} // Gfx
}

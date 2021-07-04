#ifndef XR_GFXRESOURCEMANAGER_HPP
#define XR_GFXRESOURCEMANAGER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "GfxCommon.hpp"
#include "xr/threading/Spinlock.hpp"
#include <unordered_map>

namespace xr
{
namespace Gfx
{

using InternalHandle = uint32_t;
using InternalEnum = uint32_t;

using VertexFormatRef = Ref<VertexFormat>;

//=============================================================================
struct Resource
{
  InternalHandle name = -1;
};

//=============================================================================
struct VertexBufferObject : Resource
{
  static uint32_t EncodeInstanceDataStride(InstanceDataStrideType stride)
  {
    return F_BUFFER_INSTANCE_DATA |
      (stride << (XR_BITSIZEOF(FlagType) - (1 + XR_BITSIZEOF(InstanceDataStrideType))));
  }

  VertexFormatHandle hFormat;
  FlagType flags = F_BUFFER_NONE;
  InternalEnum  target;

  InstanceDataStrideType DecodeInstanceDataStride() const
  {
    XR_ASSERT(Gfx, CheckAllMaskBits(flags, F_BUFFER_INSTANCE_DATA));
    return InstanceDataStrideType((flags & ~F_BUFFER_INSTANCE_DATA) >>
      (XR_BITSIZEOF(FlagType) - (1 + XR_BITSIZEOF(InstanceDataStrideType))));
  }
};

//=============================================================================
struct IndexBufferObject : Resource
{
  uint8_t indexSize = 0;
  FlagType flags = F_BUFFER_NONE;
  InternalEnum type;
};

//=============================================================================
struct Texture: Resource
{
  TextureInfo info;
  InternalEnum target;
};

using TextureRef = Ref<Texture>;

//=============================================================================
struct FrameBufferObject : Resource
{
  TextureHandle hTextures[8];
  uint8_t numTextures;
  uint8_t numColorAttachments;
  bool    ownTextures;
};

//==============================================================================
struct Uniform
{
  UniformType type;
  uint8_t     arraySize;
  std::string name;
};

using UniformRef = Ref<Uniform>;

//=============================================================================
struct Shader : Resource
{
  ShaderType type;
};

using ShaderRef = Ref<Shader>;

//=============================================================================
struct Program : Resource
{
  enum { kAttributeCount = size_t(Attribute::kCount) };

  ShaderHandle    hVertex;
  ShaderHandle    hFragment;
  bool            linked = false;
  int             numActiveAttribs = 0;
  uint8_t         activeAttribs[kAttributeCount]; // ordinal to XR attrib id
  InternalHandle  attribLoc[kAttributeCount]; // XR attrib id to GLSL location
  uint8_t         unboundAttribs[kAttributeCount]; // bound from VertexFormat
  InternalHandle  instanceDataLoc[kMaxInstanceData + 1]; // additional space for -1 terminator
  ConstBuffer*  uniforms = nullptr;
};

//=============================================================================
class ResourceManager
{
public:
  // types
  using VertexFormats = ServicedArray<VertexFormatRef, 64>;
  using VertexBufferObjects = ServicedArray<VertexBufferObject, 4096>;
  using IndexBufferObjects = ServicedArray<IndexBufferObject, 4096>;
  using Textures = ServicedArray<TextureRef, 1024, 3>;
  using FrameBufferObjects = ServicedArray<FrameBufferObject, 256, 1>;
  using Uniforms = ServicedArray<UniformRef, 1024>;
  using Shaders = ServicedArray<ShaderRef, 512>;
  using Programs = ServicedArray<Program, 512>;

  // structors
  ~ResourceManager();

  // general
  Spinlock& GetLock();

  VertexFormatHandle RegisterVertexFormat(VertexFormat const& format);
  void Release(VertexFormatHandle h);

  VertexFormats& GetVertexFormats()
  {
    return mVertexFormats;
  }

  VertexBufferObjects& GetVbos()
  {
    return mVbos;
  }

  IndexBufferObjects& GetIbos()
  {
    return mIbos;
  }

  Textures& GetTextures()
  {
    return mTextures;
  }

  FrameBufferObjects& GetFbos()
  {
    return mFbos;
  }

  UniformHandle CreateUniform(char const* name, UniformType type, uint8_t arraySize);

  UniformHandle FindUniform(char const* name);

  uint32_t GetUniformDataSize(UniformHandle h, uint8_t numElems) const;

  Uniforms& GetUniforms()
  {
    return mUniforms;
  }

  void** GetUniformData()
  {
    return mUniformData;
  }

  void Release(UniformHandle h);

  Shaders& GetShaders()
  {
    return mShaders;
  }

  Programs& GetPrograms()
  {
    return mPrograms;
  }

private:
  // data
  Spinlock mLock;

  VertexFormats mVertexFormats;
  std::unordered_map<uint32_t, VertexFormatHandle> mVertexFormatHandles;

  VertexBufferObjects mVbos;
  IndexBufferObjects mIbos;
  Textures mTextures;
  FrameBufferObjects mFbos;

  Uniforms mUniforms;
  void* mUniformData[decltype(mUniforms)::kSize];
  std::unordered_map<uint32_t, UniformHandle> mUniformHandles;

  Shaders mShaders;
  Programs mPrograms;
};

}
}

#endif  // XR_GfxResourceManager_HPP

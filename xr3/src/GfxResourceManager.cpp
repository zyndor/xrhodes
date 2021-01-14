//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "GfxResourceManager.hpp"
#include "xr/Hash.hpp"

namespace xr
{
namespace Gfx
{

//==============================================================================
ResourceManager::~ResourceManager()
{
#define REPORT_LEAKS(name) XR_TRACEIF(Gfx, m##name##s.server.GetNumAcquired() > 0,\
  ("WARNING: %d %s handles leaked.", m##name##s.server.GetNumAcquired(), #name))
  REPORT_LEAKS(VertexFormat);
  REPORT_LEAKS(Vbo);
  REPORT_LEAKS(Ibo);
  REPORT_LEAKS(Texture);
  REPORT_LEAKS(Fbo);
  REPORT_LEAKS(Uniform);
  REPORT_LEAKS(Shader);
  REPORT_LEAKS(Program);
#undef REPORT_LEAKS
}

//==============================================================================
Spinlock& ResourceManager::GetLock()
{
  return mLock;
}

//==============================================================================
VertexFormatHandle ResourceManager::RegisterVertexFormat(VertexFormat const& format)
{
  uint32_t const hash = format.CalculateHash();
  auto iFind = mVertexFormatHandles.find(hash);
  VertexFormatHandle hFormat;
  if (iFind != mVertexFormatHandles.end())
  {
    hFormat = iFind->second;
    ++(mVertexFormats[hFormat.id].refCount);
  }
  else
  {
    hFormat.id = uint16_t(mVertexFormats.server.Acquire());
    VertexFormatRef& vfr = mVertexFormats[hFormat.id];
    vfr.inst = format;
    vfr.refCount = 1;
  }
  return hFormat;
}

//==============================================================================
void ResourceManager::Release(VertexFormatHandle h)
{
  VertexFormatRef& vfr = mVertexFormats[h.id];
  XR_ASSERT(Gfx, vfr.refCount > 0);
  --vfr.refCount;
  if (vfr.refCount == 0)
  {
    uint32_t hash = vfr.inst.CalculateHash();
    mVertexFormatHandles.erase(hash);

    mVertexFormats.server.Release(h.id);
  }
}

//==============================================================================
UniformHandle ResourceManager::CreateUniform(char const* name, UniformType type,
  uint8_t arraySize)
{
  uint32_t const hash = Hash::String32(name);
  auto iFind = mUniformHandles.find(hash);
  UniformHandle h;
  bool existing = iFind != mUniformHandles.end();
  if (existing)
  {
    // Get existing uniform
    h = iFind->second;
  }
  else
  {
    // Create uniform anew.
    h.id = uint16_t(mUniforms.server.Acquire());
  }

  UniformRef& ur = mUniforms[h.id];
  XR_ASSERTMSG(Gfx, !existing || type == ur.inst.type,
    ("Uniform '%s' type (%d) mismatches already registered type (%d)", name,
      ur.inst.type, type));
  XR_ASSERTMSG(Gfx, !existing || arraySize <= ur.inst.arraySize,
    ("Uniform '%s' array size (%d) exceeds registered size (%d).", name, arraySize,
      ur.inst.arraySize));
  if (!existing)
  {
    Uniform& u = ur.inst;
    u.name = name;
    u.type = type;
    u.arraySize = std::max(arraySize, static_cast<decltype(arraySize)>(1));

    size_t requiredBytes = u.arraySize * Const::kUniformTypeSize[uint8_t(u.type)];
    void*& data = mUniformData[h.id];
    data = malloc(requiredBytes);
    std::memset(data, 0x00, requiredBytes);

    ur.refCount = 1;
    mUniformHandles[hash] = h;
  }
  else
  {
    ++ur.refCount;
  }

  return h;
}

//==============================================================================
UniformHandle ResourceManager::FindUniform(char const* name)
{
  auto iFind = mUniformHandles.find(Hash::String32(name));
  return iFind != mUniformHandles.end() ? iFind->second : UniformHandle();
}

//==============================================================================
uint32_t ResourceManager::GetUniformDataSize(UniformHandle h, uint8_t numElems) const
{
  auto const& ur = mUniforms.data[h.id];
  XR_ASSERT(Gfx, ur.refCount > 0);
  auto const& u = ur.inst;
  uint32_t size = Const::kUniformTypeSize[static_cast<uint8_t>(u.type)];
  return size * std::min(u.arraySize, numElems);
}

//==============================================================================
void ResourceManager::Release(UniformHandle h)
{
  UniformRef& ur = mUniforms[h.id];
  XR_ASSERT(Gfx, ur.refCount > 0);
  --ur.refCount;
  if (ur.refCount == 0)
  {
    free(mUniformData[h.id]);
    mUniformData[h.id] = nullptr;

    ur.inst = Uniform();

    mUniforms.server.Release(h.id);
  }
}

} // Gfx
}

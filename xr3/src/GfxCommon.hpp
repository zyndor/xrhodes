#ifndef XR_GFXAUX_HPP
#define XR_GFXAUX_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Gfx.hpp"
#include "xr/types/fundamentals.hpp"
#include "xr/memory/IndexServer.hpp"
#include "xr/memory/memory.hpp"

#define XR_ALIGN16(val) xr::Align<uint32_t>(val, 16)

namespace xr
{
namespace Gfx
{

//=============================================================================
class Const
{
  XR_NONOBJECT_DECL(Const)

public:
  static const uint32_t kUniformTypeSize[];
  static const char* const kAttributeName[];
  static const char* const kInstanceDataName[];
};

//=============================================================================
enum class AttachmentType : uint8_t
{
  Color,
  Depth,
  Stencil,
  DepthStencil
};

//==============================================================================
template <typename T, typename SizeType = uint16_t>
struct Ref
{
  T inst;
  SizeType refCount;
};

//==============================================================================
// TODO: ServicedArrayCore or dynamic allocation
template <typename T, uint32_t n, uint32_t exempt = 0>
class ServicedArray
{
  XR_NONCOPY_DECL(ServicedArray)

public:
  static const uint32_t kSize = n;

  static uint32_t size()
  {
    return kSize;
  }

  T data[kSize];
  IndexServer server;

  ServicedArray()
  : server(kSize - exempt)
  {}

  T& operator[](uint32_t i)
  {
    XR_ASSERT(ServicedArray, i < server.GetNumActive() || n - 1 - i < exempt);
    return data[i];
  }

  T const& operator[](uint32_t i) const
  {
    XR_ASSERT(ServicedArray, i < server.GetNumActive() || n - 1 - i < exempt);
    return data[i];
  }
};

//==============================================================================
class ConstBuffer
{
public:
  static ConstBuffer* Create(uint32_t size)
  {
    size_t alignedSize = XR_ALIGN16(size);
    void* mem = malloc(alignedSize);
    return new (mem) ConstBuffer(size);
  }

  static void Destroy(ConstBuffer* cb)
  {
    cb->~ConstBuffer();
    free(cb);
  }

  bool AtEnd() const
  {
    XR_ASSERT(ConstBuffer, m_pos < m_size);
    return m_buffer[m_pos] == uint8_t(UniformType::kCount);
  }

  void Reset(uint32_t pos = 0)
  {
    m_pos = pos;
  }

  void Read(UniformType& t, uint8_t& arraySize, uint16_t& loc, bool& ref)
  {
    uint32_t pack = 0;
    Read(pack);

    uint8_t refType = pack >> 24;
    ref = (refType & 0x80) != 0;
    t = static_cast<UniformType>(refType & 0x7f);
    arraySize = (pack >> 16) & 0x00ff;
    loc = pack & 0xffff;
  }

  void ReadHandle(UniformType& t, uint8_t& arraySize, uint16_t& loc, UniformHandle& h)
  {
    bool ref;
    Read(t, arraySize, loc, ref);
    XR_ASSERTMSG(ConstBuffer, ref, ("Expected handle - got copy."));
    Read(h);
  }

  template <typename T>
  void Read(T& val)
  {
    union { uint8_t* c; T* val; } p;
    p.c = ReadBytes(sizeof(T));
    val = *p.val;
  }

  uint8_t* ReadBytes(uint32_t size)
  {
    XR_ASSERTMSG(ConstBuffer, m_pos + size < m_size,
      ("Reading past end in ConstBuffer %p: overflow by %d bytes", this, m_pos + size - m_size));
    uint8_t* p = GetPtr();
    m_pos += size;
    return p;
  }

  void WriteHandle(UniformType t, uint8_t arraySize, uint16_t loc, UniformHandle h)
  {
    XR_ASSERT(ConstBuffer, m_pos + sizeof(uint32_t) + sizeof(UniformHandle) < m_size);
    uint32_t refTypeSizeLoc = (((uint8_t(t) & 0x7f) | 0x80) << 24) // 1 bits ref (1), 7 bits type
      | (arraySize << 16) // 8 bits size
      | loc; // 16 bits loc
    Write(refTypeSizeLoc);
    Write(h);
  }

  void WriteData(UniformType t, uint8_t arraySize, uint16_t loc, void const* data)
  {
    const uint32_t dataSize = Const::kUniformTypeSize[uint8_t(t)] * arraySize;
    XR_ASSERT(ConstBuffer, m_pos + sizeof(uint32_t) + dataSize < m_size);
    uint32_t refTypeSizeLoc = ((uint8_t(t) & 0x7f) << 24) // 1 bits ref (0), 7 bits type
      | (arraySize << 16) // 8 bits size
      | loc; // 16 bits loc
    Write(refTypeSizeLoc);
    WriteBytes(data, dataSize);
  }

  template <typename T>
  void Write(T data)
  {
    WriteBytes(&data, sizeof(T));
  }

  void WriteBytes(void const* data, uint32_t size)
  {
    XR_ASSERT(ConstBuffer, m_pos + size < m_size);
    uint8_t* p = GetPtr();
    std::memcpy(p, data, size);
    m_pos += size;
  }

  void Finalize()
  {
    Write(uint8_t(UniformType::kCount));
    Reset();
  }

private:
  ConstBuffer(uint32_t size)
  : m_pos(0),
    m_size(size - sizeof(m_pos) - sizeof(m_size))
  {}

  uint8_t* GetPtr()
  {
    return m_buffer + m_pos;
  }

  uint32_t m_pos;
  uint32_t m_size;
  uint8_t  m_buffer[16 - (sizeof(decltype(m_pos)) + sizeof(decltype(m_size)))]; // buffer includes padding to 16 bytes - actual size will vary. Refer to Create().
};
static_assert(sizeof(ConstBuffer) == 16, "sizeof(ConstBuffer) must be 16 bytes.");

} // Gfx
}

#endif

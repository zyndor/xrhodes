//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/memory/Pool.hpp"
#include "xr/debug.hpp"

namespace xr
{

//==============================================================================
void* Pool::Allocate(size_t size, void* userData)
{
  Pool*  pool = static_cast<Pool*>(userData);
  return pool->Allocate(size);
}

//==============================================================================
void  Pool::Deallocate(void* /*buffer*/, void* /*userData*/)
{
  // sweet nothings
}

//==============================================================================
Pool::Pool()
: m_buffer(nullptr),
  m_isAuto(false),
  m_next(nullptr),
  m_end(nullptr)
{}

//==============================================================================
Pool::Pool(size_t size)
: m_buffer(new Byte[size]),
  m_isAuto(true),
  m_frames(),
  m_next(m_buffer),
  m_end(m_buffer)
{
  m_end += size;
}

//==============================================================================
Pool::Pool(size_t size, bool isAuto, Byte* parBuffer)
: m_buffer(nullptr),
  m_isAuto(false),
  m_frames(),
  m_next(nullptr),
  m_end(nullptr)
{
  SetBuffer(size, isAuto, parBuffer);
}

//==============================================================================
Pool::~Pool()
{
  SetBuffer(0, false, nullptr);
}

//==============================================================================
Byte* Pool::RegainBuffer()
{
  Byte* buffer = m_buffer;
  m_buffer = nullptr;
  m_isAuto = false;
  m_next = nullptr;
  m_end = nullptr;
  m_frames.clear();
  return buffer;
}

//==============================================================================
void  Pool::SetBuffer(size_t size, bool isAuto, Byte* parBuffer)
{
  XR_ASSERTMSG(Pool, m_buffer == m_next,
    ("Already allocated from pool; those objects will be invalidated."));
  if (m_isAuto)
  {
    delete[] m_buffer;
  }

  if (parBuffer == nullptr && size > 0)
  {
    XR_ASSERT(Pool, isAuto);
    parBuffer = new Byte[size];
    isAuto = true;
  }

  m_buffer = static_cast<Byte*>(parBuffer);
  m_isAuto = isAuto;

  m_end = m_buffer + size;
  m_next = m_buffer;
}

//==============================================================================
void* Pool::Allocate(size_t numBytes)
{
  XR_ASSERT(Pool, m_buffer != nullptr);

  Byte  *next(m_next + numBytes);
  if (next <= m_end)
  {
    Byte  *buffer(m_next);
    m_next = next;
    return buffer;
  }
  else
  {
    return nullptr;
  }
}

//==============================================================================
void  Pool::Flush()
{
  m_next = m_frames.empty() ? m_buffer : m_frames.back();
}

//==============================================================================
void  Pool::Push()
{
  m_frames.push_back(m_next);
}

//==============================================================================
void  Pool::Pop()
{
  XR_ASSERT(Pool, !m_frames.empty());
  Flush();
  m_frames.pop_back();
}

} // xr

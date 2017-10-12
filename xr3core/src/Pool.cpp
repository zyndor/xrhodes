//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Pool.hpp"
#include "XR/debug.hpp"

namespace XR
{

//==============================================================================
void* Pool::Allocate(size_t size, void* pUser)
{
  Pool*  pPool(static_cast<Pool*>(pUser));
  return pPool->Allocate(size);
}

//==============================================================================
void  Pool::Deallocate(void* pMem, void* pUser)
{
  // sweet nothings
}

//==============================================================================
Pool::Pool()
: m_parBuffer(0),
  m_isAuto(false),
  m_pEnd(nullptr),
  m_pNext(nullptr)
{}

//==============================================================================
Pool::Pool(size_t size)
: m_parBuffer(new Byte[size]),
  m_isAuto(true),
  m_frames(),
  m_pNext(m_parBuffer),
  m_pEnd(m_parBuffer)
{
  XR_ASSERT(Pool, size >= 0);
  m_pEnd += size;
}

//==============================================================================
Pool::Pool(size_t size, bool isAuto, Byte* parBuffer)
: m_parBuffer(0),
  m_isAuto(false),
  m_frames(),
  m_pNext(0),
  m_pEnd(0)
{
  XR_ASSERT(Pool, size >= 0);
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
  Byte* buffer = m_parBuffer;
  m_parBuffer = nullptr;
  m_isAuto = false;
  m_pNext = nullptr;
  m_pEnd = nullptr;
  m_frames.clear();
  return buffer;
}

//==============================================================================
void  Pool::SetBuffer(size_t size, bool isAuto, Byte* parBuffer)
{
  XR_ASSERTMSG(Pool, m_parBuffer == m_pNext,
    ("Already allocated from pool; those objects will be invalidated."));
  if (m_isAuto)
  {
    delete[] m_parBuffer;
  }

  XR_ASSERT(Pool, size >= 0);
  if (parBuffer == nullptr && size > 0)
  {
    XR_ASSERT(Pool, isAuto);
    parBuffer = new Byte[size];
    isAuto = true;
  }

  m_parBuffer = static_cast<Byte*>(parBuffer);
  m_isAuto = isAuto;

  m_pEnd = m_parBuffer + size;
  m_pNext = m_parBuffer;
}

//==============================================================================
void* Pool::Allocate(size_t numBytes)
{
  XR_ASSERT(Pool, numBytes >= 0);
  XR_ASSERT(Pool, m_parBuffer != 0);

  Byte  *pNext(m_pNext + numBytes);
  if (pNext <= m_pEnd)
  {
    Byte  *pMem(m_pNext);
    m_pNext = pNext;
    return pMem;
  }
  else
  {
    return nullptr;
  } 
}

//==============================================================================
void  Pool::Flush()
{
  m_pNext = m_frames.empty() ? m_parBuffer : m_frames.back();
}

//==============================================================================
void  Pool::Push()
{
  m_frames.push_back(m_pNext);
}

//==============================================================================
void  Pool::Pop()
{
  XR_ASSERT(Pool, !m_frames.empty());
  Flush();
  m_frames.pop_back();
}

} // XR
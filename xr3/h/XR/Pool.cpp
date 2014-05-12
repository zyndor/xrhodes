//
// Nuclear Heart Games
// XRhodes
//
// MemPool.cpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date  04/08/2011
//
//==============================================================================
#include "Pool.hpp"

namespace XR
{

//==============================================================================
///@brief Constructs an empty Pool.
Pool::Pool()
: m_parBuffer(0),
  m_isAuto(false),
  m_pEnd(0),
  m_pNext(0)
{}

//==============================================================================
///@brief Constructs a Pool with a buffer of @a size bytes, allocated with
/// new[]. The Pool takes ownership of this memory.
Pool::Pool(int size)
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
///@brief Constructs a Pool with a buffer of @a size bytes. @a isAuto
/// signifies that the Pool will delete[] the memory on destruction or
/// SetBuffer(). If a size > 0 is specified with a @a parBuffer of 0,
/// the buffer will be allocated with new[].
Pool::Pool(int size, bool isAuto, Byte* parBuffer)
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
  SetBuffer(0, false, 0);
}

//==============================================================================
///@brief Returns ownership of the buffer. Unless you know what you're doing,
/// you should not really need this.
///@note  No further allocations will be possible through the Pool until a new
/// buffer is set with SetBuffer().
void* Pool::RegainBuffer()
{
  Byte* pReturn(m_parBuffer);
  m_parBuffer = 0;
  m_isAuto = false;
  m_pNext = 0;
  m_pEnd = 0;
  m_frames.Reset();
  return m_parBuffer;
}

//==============================================================================
///@brief Sets a buffer for the Pool. @a isAuto signifies ownership transfer
/// of @a parBuffer to the pool. This memory will be delete[]d on destruction
/// of the Pool or the next SetBuffer() call, whichever is sooner. If a @a size
/// > 0 is specified with a @a parBuffer of 0, the buffer will be allocated
/// with new[] and @a isAuto will be set to true.
///@note  You have to make sure that @a parBuffer (unless is a 0 pointer), does
/// span @a size bytes.
void  Pool::SetBuffer(int size, bool isAuto, void* parBuffer)
{
  XR_ASSERTMSG(XR::Pool, m_parBuffer == m_pNext,
    ("Already allocated from pool; those objects will be invalidated."));
  if (m_isAuto)
  {
    delete[] m_parBuffer;
  }

  XR_ASSERT(Pool, size >= 0);
  if (parBuffer == 0 && size > 0)
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
///@return  A @a size bytes chunk of memory, or 0 if there was not enough
/// memory left in the pool.
void* Pool::Allocate(int numBytes)
{
  XR_ASSERT(Pool::Pool, numBytes >= 0);
  XR_ASSERT(Pool::Pool, m_parBuffer != 0);

  Byte  *pNext(m_pNext + numBytes);
  if (pNext <= m_pEnd)
  {
    Byte  *pMem(m_pNext);
    m_pNext = pNext;
    return pMem;
  }
  else
  {
    return 0;
  } 
}

//==============================================================================
///@brief Returns the allocation pointer to the beginning of the pool, making
/// its memory available to be allocated again.
///@note  You will have to make sure that the object on the frame are
/// destructed properly beforehand.
void  Pool::Flush()
{
  m_pNext = m_frames.IsEmpty() ? m_parBuffer : m_frames.Top();
}

//==============================================================================
///@brief Creates a frame beginning with the point of the next allocation.
///@brief Tou will have to make sure that the number of frames don't exceed
/// the maximum.
void  Pool::Push()
{
  XR_ASSERT(Pool::Pool, CanPush());
  m_frames.Push(m_pNext);
}

//==============================================================================
///@brief Flushes and removes the last frame.
///@note  You will have to make sure that there are frames to begin with.
///@note  The next allocation will be made as part of and at the end of,
/// the previous frame.
///@note  You will have to make sure that the object on the frame are
/// destructed properly beforehand.
void  Pool::Pop()
{
  XR_ASSERT(Pool::Pool, GetNumFrames() > 0);
  Flush();
  m_frames.Pop();
}

} // XR
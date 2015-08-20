//
// Nuclear Heart Games
// XRhodes
//
// Pool.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    04/08/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_POOL_HPP
#define XR_POOL_HPP

#include <list>
#include "memory.hpp"

namespace XR
{

//==============================================================================
///@brief The single byte type used throughout XRhodes.
typedef unsigned char  Byte;

//==============================================================================
///@brief Lightweight memory pool class for session allocations, useful where
/// you only want the memory back at the end of the session and in whole:
/// Pool does not keep track of objects that have been destroyed.
///@note  You should never delete memory that comes from a Pool.
class Pool
{
public:
  // static
  static const int  kMaxFrames = 8;

  static void* Allocate(size_t size, void* pUser);
  static void  Deallocate(void* pMem, void* pUser);

  // structors
  Pool();

  ///@brief Constructs a Pool with a buffer of @a size bytes, allocated with
  /// new[]. The Pool takes ownership of this memory.
  explicit Pool(size_t size);

  ///@brief Constructs a Pool with a buffer of @a size bytes. @a isAuto
  /// signifies that the Pool will delete[] the memory on destruction or
  /// SetBuffer(). If a size > 0 is specified with a @a parBuffer of 0,
  /// the buffer will be allocated with new[].
  Pool(size_t size, bool isAuto, Byte* parBuffer); // ownership transfer

  ~Pool();

  // general use
  bool  HasBuffer() const;

  ///@brief Returns ownership of the buffer and resets the Pool.
  ///@note  No further allocations will be possible through the Pool until a
  /// new buffer is set with SetBuffer().
  void* RegainBuffer();

  ///@brief Sets a buffer for the Pool. @a isAuto signifies ownership transfer
  /// of @a parBuffer to the pool. This memory will be delete[]d on destruction
  /// of the Pool or the next SetBuffer() call, whichever is sooner. If a
  /// @a size > 0 is specified with a @a parBuffer of 0, the buffer will be
  /// allocated with new[] and @a isAuto will be set to true.
  ///@note  You have to make sure that @a parBuffer (unless is a 0 pointer),
  /// does span @a size bytes.
  void  SetBuffer(size_t size, bool isAuto, void* parBuffer); // ownership transfer

  ///@return  A @a size bytes chunk of memory, or 0 if there was not enough
  /// memory left in the pool.
  void* Allocate(size_t numBytes);
  
  ///@brief Returns the allocation pointer to the beginning of the frame,
  /// making its memory available to be allocated again.
  ///@note  You will have to make sure that the objects on the frame are
  /// destructed properly beforehand.
  void  Flush();

  ///@return  The size of all allocations made from the Pool, in bytes.
  int CalculateSize() const;

  ///@return  The size of all allocations made from the current frame, in
  /// bytes.
  int CalculateFrameSize() const;

  ///@return  The amount of free memory in the Pool, in bytes.
  int CalculateFree() const;

  ///@return  The number of frames on the Pool.
  int GetNumFrames() const;
  
  ///@brief Creates a frame beginning at the point of the next allocation.
  void  Push();

  ///@brief Flushes and removes the last frame.
  ///@note  You will have to make sure that there are frames to begin with.
  ///@note  The next allocation will be made as part of and at the end of,
  /// the previous frame.
  ///@note  You will have to make sure that the object on the frame are
  /// destructed properly beforehand.
  void  Pop();

protected:
  // types
  typedef std::list<Byte*> FrameStack;

  // data
  Byte* m_parBuffer;
  bool  m_isAuto;

  FrameStack  m_frames;
  Byte*       m_pNext;
  Byte*       m_pEnd;
};

//==============================================================================
// implementation
//==============================================================================
inline
bool  Pool::HasBuffer() const
{
  return m_parBuffer != 0;
}

//==============================================================================
inline
int Pool::CalculateSize() const
{
  return m_pEnd - m_parBuffer;
}

//==============================================================================
inline
int Pool::CalculateFrameSize() const
{
  return m_pEnd - (m_frames.empty() ? m_parBuffer : m_frames.back());
}

//==============================================================================
inline
int Pool::CalculateFree() const
{
  return m_pEnd - m_pNext;
}

//==============================================================================
inline
int Pool::GetNumFrames() const
{
  return int(m_frames.size());
}

} // XR

//==============================================================================
#define XR_POOL_ALLOC(type, num, pool) static_cast<type*>((pool).Allocate(sizeof(type) * (num)))

#define XR_POOL_ALLOC_INIT(type, num, pool) new (XR_POOL_ALLOC(type, (num), (pool))) type()

#endif  // XR_POOL_HPP

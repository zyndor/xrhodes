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

#include "HardStack.hpp"

#define TO_KBYTES(a) ((a) * 1024)
#define TO_MBYTES(a) (TO_KBYTES(a) * 1024)

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

  // structors
  Pool();

  ///@brief Constructs a Pool with a buffer of @a size bytes, allocated with
  /// new[]. The Pool takes ownership of this memory.
  explicit Pool(int size);

  ///@brief Constructs a Pool with a buffer of @a size bytes. @a isAuto
  /// signifies that the Pool will delete[] the memory on destruction or
  /// SetBuffer(). If a size > 0 is specified with a @a parBuffer of 0,
  /// the buffer will be allocated with new[].
  Pool(int size, bool isAuto, Byte* parBuffer); // ownership transfer

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
  void  SetBuffer(int size, bool isAuto, void* parBuffer); // ownership transfer

  ///@return  A @a size bytes chunk of memory, or 0 if there was not enough
  /// memory left in the pool.
  void* Allocate(int numBytes);
  
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
  
  ///@return  The number of legal Push() calls that you can make on this Pool.
  int CanPush() const;

  ///@brief Creates a frame beginning at the point of the next allocation.
  ///@note  You will have to make sure that the number of frames don't exceed
  /// kMaxFrames.
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
  typedef HardStack<Byte*, kMaxFrames> FrameStack;

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
  return m_pEnd - (GetNumFrames() > 0 ? m_frames.Top() : m_parBuffer);
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
  return m_frames.NumElements();
}

//==============================================================================
inline
int Pool::CanPush() const
{
  return FrameStack::kMaxSize - m_frames.NumElements();
}

} // XR

//==============================================================================
#define XR_POOL_ALLOC(type, num, pool) static_cast<type*>((pool).Allocate(sizeof(type) * (num)))

#define XR_POOL_ALLOC_INIT(type, num, pool) new (XR_POOL_ALLOC(type, (num), (pool))) type()

#endif  //XR_POOL_HPP

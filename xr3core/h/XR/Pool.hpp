#ifndef XR_POOL_HPP
#define XR_POOL_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "memory.hpp"
#include <list>

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

  static void* Allocate(size_t size, void* userData);
  static void  Deallocate(void* buffer, void* userData);

  // structors
  Pool();

  ///@brief Constructs a Pool with a buffer of @a size bytes, allocated with
  /// new[]. The Pool takes ownership of this memory.
  explicit Pool(size_t size);

  ///@brief Constructs a Pool with a buffer of @a size bytes. @a isAuto
  /// signifies that the Pool will delete[] the memory on destruction or
  /// SetBuffer(). If a size > 0 is specified with a @a buffer of 0,
  /// the buffer will be allocated with new[].
  Pool(size_t size, bool isAuto, Byte* buffer); // ownership transfer

  ~Pool();

  // general use
  bool  HasBuffer() const;

  ///@brief Returns ownership of the buffer and resets the Pool.
  ///@note  No further allocations will be possible through the Pool until a
  /// new buffer is set with SetBuffer().
  Byte* RegainBuffer();

  ///@brief Sets a buffer for the Pool. @a isAuto signifies ownership transfer
  /// of @a buffer to the pool. This memory will be delete[]d on destruction
  /// of the Pool or the next SetBuffer() call, whichever is sooner. If a
  /// @a size > 0 is specified with a @a buffer of 0, the buffer will be
  /// allocated with new[] and @a isAuto will be set to true.
  ///@note  You have to make sure that @a buffer (unless is a 0 pointer),
  /// does span @a size bytes.
  void  SetBuffer(size_t size, bool isAuto, Byte* buffer); // ownership transfer

  ///@return  A @a size bytes chunk of memory, or 0 if there was not enough
  /// memory left in the pool.
  void* Allocate(size_t numBytes);

  ///@brief Returns the allocation pointer to the beginning of the frame,
  /// making its memory available to be allocated again.
  ///@note  You will have to make sure that the objects on the frame are
  /// destructed properly beforehand.
  void  Flush();

  ///@return  The total capacity of the pool, in bytes.
  size_t CalculateSize() const;

  ///@return  The size of all allocations made from the Pool, in bytes.
  size_t CalculateUsed() const;

  ///@return  The size of all allocations made from the current frame, in
  /// bytes.
  size_t CalculateFrameSize() const;

  ///@return  The amount of free memory in the Pool, in bytes.
  size_t CalculateFree() const;

  ///@return  The number of frames on the Pool.
  size_t GetNumFrames() const;

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
  Byte* m_buffer;
  bool  m_isAuto;

  FrameStack  m_frames;
  Byte*       m_next;
  Byte*       m_end;
};

//==============================================================================
// implementation
//==============================================================================
inline
bool  Pool::HasBuffer() const
{
  return m_buffer != nullptr;
}

//==============================================================================
inline
size_t Pool::CalculateSize() const
{
  return m_end - m_buffer;
}

//==============================================================================
inline
size_t Pool::CalculateUsed() const
{
  return m_next - m_buffer;
}

//==============================================================================
inline
size_t Pool::CalculateFrameSize() const
{
  return m_next - (m_frames.empty() ? m_buffer : m_frames.back());
}

//==============================================================================
inline
size_t Pool::CalculateFree() const
{
  return m_end - m_next;
}

//==============================================================================
inline
size_t Pool::GetNumFrames() const
{
  return m_frames.size();
}

} // XR

//==============================================================================
#define XR_POOL_ALLOC(type, num, pool) static_cast<type*>((pool).Allocate(sizeof(type) * (num)))

#define XR_POOL_ALLOC_INIT(type, num, pool) new (XR_POOL_ALLOC(type, (num), (pool))) type()

#endif  // XR_POOL_HPP

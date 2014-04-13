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
  explicit Pool(int size);
  Pool(int size, bool isAuto, Byte* parBuffer); // ownership transfer
  ~Pool();

  // general use
  void* RegainBuffer();
  void  SetBuffer(int size, bool isAuto, void* parBuffer); // ownership transfer

  void* Allocate(int numBytes);
  
  void  Flush();

  int CalculateSize() const;
  int CalculateFrameSize() const;
  int CalculateFree() const;

  int GetNumFrames() const;
  int CanPush() const;

  void  Push();
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
/*
  implementation
 */
//==============================================================================
///@return  The size of all allocations from the Pool, in bytes.
inline
int Pool::CalculateSize() const
{
  return m_pEnd - m_parBuffer;
}

//==============================================================================
///@return  The size of all allocations from the current frame, in bytes.
inline
int Pool::CalculateFrameSize() const
{
  return m_pEnd - (GetNumFrames() > 0 ? m_frames.Top() : m_parBuffer);
}

//==============================================================================
///@return  The amount of free memory in the Pool, in bytes.
inline
int Pool::CalculateFree() const
{
  return m_pEnd - m_pNext;
}

//==============================================================================
///@return  The number of legal to Pop() calls that you can make on this Pool.
inline
int Pool::GetNumFrames() const
{
  return m_frames.NumElements();
}

//==============================================================================
///@return  The number of legal to Push() calls that you can make on this Pool.
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

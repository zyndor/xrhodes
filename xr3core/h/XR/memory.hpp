#ifndef XR_MEMORY_HPP
#define XR_MEMORY_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "debug.hpp"
#include <stdint.h>
#include <stdlib.h>

namespace XR
{

//==============================================================================
typedef void*(*AllocateCallback)(size_t size, void* userData);
typedef void(*DeallocateCallback)(void* pMemory, void* userData);

//==============================================================================
class Allocator
{
public:
  // static
  static void* Allocate(size_t size, void* userData)
  {
    return static_cast<Allocator*>(userData)->Allocate(size);
  }

  static void Deallocate(void* memory, void* userData)
  {
    static_cast<Allocator*>(userData)->Deallocate(memory);
  }

  // structors
  virtual ~Allocator() {}

  // virtual
  virtual void* Allocate(size_t numBytes) =0;
  virtual void Deallocate(void* memory) =0;
};

//==============================================================================
class Mallocator : public Allocator
{
public:
  virtual void* Allocate(size_t numBytes)
  {
    return malloc(numBytes);
  }

  virtual void Deallocate(void* memory)
  {
    free(memory);
  }
};

//==============================================================================
///@brief Align @a value to given @a alignment, which must be a power of two.
template <typename T = size_t>
T Align(T value, T alignment);

//==============================================================================
// implementation
//==============================================================================
template <typename T>
inline
T Align(T value, T alignment)
{
  XR_ASSERT(Align, alignment > 0);
  const T alignmentLess1 = alignment - 1;
  XR_ASSERT(Align, (alignment & alignmentLess1) == 0);
  return value + alignment - (value & alignmentLess1);
}

} // XR

//==============================================================================
#define XR_KBYTES(a) ((a) << 10)  // * 1024
#define XR_MBYTES(a) (XR_KBYTES(a) << 10)

#endif  // XR_MEMORY_HPP

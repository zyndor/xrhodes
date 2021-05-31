#ifndef XR_MEMORY_HPP
#define XR_MEMORY_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/debug.hpp"
#include <stdint.h>
#include <stdlib.h>
#include <new>

namespace xr
{

//==============================================================================
typedef void*(*AllocateCallback)(size_t size, void* userData);
typedef void(*DeallocateCallback)(void* buffer, void* userData);

//==============================================================================
class Allocator
{
public:
  // static
  [[nodiscard]] static void* Allocate(size_t size, void* userData)
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
  [[nodiscard]] virtual void* Allocate(size_t numBytes) =0;
  virtual void Deallocate(void* buffer) =0;
};

//==============================================================================
class Mallocator : public Allocator
{
public:
  [[nodiscard]] void* Allocate(size_t numBytes) override
  {
    return malloc(numBytes);
  }

  void Deallocate(void* buffer) override
  {
    free(buffer);
  }
};

//==============================================================================
class GlobalNewAllocator : public Allocator
{
public:
  [[nodiscard]] void* Allocate(size_t numBytes) override
  {
    return ::operator new(numBytes, std::nothrow);
  }

  void Deallocate(void* buffer) override
  {
    ::operator delete(buffer);
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
  return (value + alignmentLess1) & ~alignmentLess1;
}

} // xr

//==============================================================================
#define XR_KBYTES(a) ((a) << 10)  // * 1024
#define XR_MBYTES(a) (XR_KBYTES(a) << 10)

#endif  // XR_MEMORY_HPP

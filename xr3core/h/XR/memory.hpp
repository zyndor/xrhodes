//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_MEMORY_HPP
#define XR_MEMORY_HPP

#include "debug.hpp"
#include <stdint.h>
#include <stdlib.h>

namespace XR
{

//==============================================================================
typedef void*(*AllocateCallback)(size_t size, void* pUser);
typedef void(*DeallocateCallback)(void* pMemory, void* pUser);

int32_t Align(int32_t value, int32_t alignment);

//==============================================================================
// implementation
//==============================================================================
inline
int32_t Align(int32_t value, int32_t alignment)
{
  XR_ASSERT(Align, alignment > 0);
  const int alignmentLess1 = alignment - 1;
  XR_ASSERT(Align, (alignment & alignmentLess1) == 0);
  return value + alignment - (value & alignmentLess1);
}

} // XR

//==============================================================================
#define XR_KBYTES(a) ((a) * 1024)
#define XR_MBYTES(a) (XR_KBYTES(a) * 1024)

#endif  // XR_MEMORY_HPP
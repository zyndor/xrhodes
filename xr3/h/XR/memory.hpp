//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_MEMORY_HPP
#define XR_MEMORY_HPP

#include  <stdlib.h>

namespace XR
{

//==============================================================================
typedef void*(*AllocateCallback)(size_t size, void* pUser);
typedef void(*DeallocateCallback)(void* pMemory, void* pUser);

} // XR

//==============================================================================
#define XR_KBYTES(a) ((a) * 1024)
#define XR_MBYTES(a) (XR_KBYTES(a) * 1024)

#endif  // XR_MEMORY_HPP
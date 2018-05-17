//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/SharedPoolAllocator.hpp"

namespace XR
{

//==============================================================================
SharedPoolAllocatorCore::SharedPoolAllocatorCore(Pool* pool) throw()
: m_pool(pool)
{}

//==============================================================================
SharedPoolAllocatorCore::~SharedPoolAllocatorCore() throw()
{}

} // XR


//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/SharedPoolAllocator.hpp"

namespace xr
{

//==============================================================================
SharedPoolAllocatorCore::SharedPoolAllocatorCore(Pool* pool) throw()
: m_pool(pool)
{}

//==============================================================================
SharedPoolAllocatorCore::~SharedPoolAllocatorCore() throw()
{}

} // xr


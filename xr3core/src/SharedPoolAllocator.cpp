//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
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


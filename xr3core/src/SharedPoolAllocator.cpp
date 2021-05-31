//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/memory/SharedPoolAllocator.hpp"

namespace xr
{

//==============================================================================
SharedPoolAllocatorCore::SharedPoolAllocatorCore(Pool* pool) noexcept
: m_pool(pool)
{}

//==============================================================================
SharedPoolAllocatorCore::~SharedPoolAllocatorCore() noexcept
{}

} // xr


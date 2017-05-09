//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/SharedPoolAllocator.hpp>

namespace XR
{

//==============================================================================
SharedPoolAllocatorCore::SharedPoolAllocatorCore(Pool* pPool) throw()
: m_pPool(pPool)
{}

//==============================================================================
SharedPoolAllocatorCore::~SharedPoolAllocatorCore() throw()
{}

} // XR


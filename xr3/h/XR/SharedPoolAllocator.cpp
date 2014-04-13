//
// Nuclear Heart Studios
// XRhodes
//
// SharedPoolAllocator.cpp
// @author Gyorgy Straub <gyorgy@nuclearheart.com>
// @date  03/12/2011
//
//==============================================================================
#include "SharedPoolAllocator.hpp"

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


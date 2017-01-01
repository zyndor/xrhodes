//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "CrossObject.hpp"

namespace XR
{

//==============================================================================
CrossObject::CrossObject()
: m_pImpl(0)
{}

//==============================================================================
CrossObject::CrossObject( void* pImpl )
: m_pImpl(pImpl)
{}

//==============================================================================
CrossObject::~CrossObject()
{}

} // XR
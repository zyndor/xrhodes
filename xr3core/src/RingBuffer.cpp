//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/RingBuffer.hpp>

namespace XR
{

RingBufferCore::RingBufferCore()
: m_front(0),
  m_back(0),
  m_elems(0)
{}

RingBufferCore::~RingBufferCore()
{}

} // XR


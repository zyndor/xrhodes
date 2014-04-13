#include  "RingBuffer.hpp"

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


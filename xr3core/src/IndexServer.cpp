//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/IndexServer.hpp>
#include <sstream>

namespace XR
{

//==============================================================================
IndexServer::IndexServer(size_t capacity)
: m_iReleased(0),
  m_ids()
{
  m_ids.reserve(capacity);
}

//==============================================================================
size_t IndexServer::Acquire()
{
  size_t i = m_ids.size();
  if (m_iReleased < i)
  {
    // grab the first known released id.
    i = m_ids[m_iReleased];
  }
  else if (i < m_ids.capacity())
  {
    // if we still have the capacity, create a new handle.
    m_ids.push_back(i);
  }
  else
  {
    // we're out of indices
    throw std::bad_alloc();
  }

  // if we were successful, we'll now shit the tracker up.
  ++m_iReleased;

  return i;
}

//==============================================================================
void IndexServer::Release(size_t i)
{
  auto iBegin = std::begin(m_ids);
  auto iEnd = iBegin + m_iReleased;
  auto iFind = std::find(iBegin, iEnd, i);
  if (iFind != iEnd)
  {
    // Swap index with most recently activated one. Note that this makes our
    // registry of released indices like a stack - new releases always go
    // on the top.
    std::swap(*iFind, *(m_ids.begin() + (m_iReleased - 1)));
    // Bump dealloc marker down, invalidating the last element (which is the one we've just released).
    --m_iReleased;
  }
  else
  {
    std::ostringstream str;
    str << "Invalid deallocation of id " << i << ".";
    throw std::logic_error(str.str());
  }
}

} // XR
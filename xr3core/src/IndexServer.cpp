//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/memory/IndexServer.hpp"
#include <sstream>
#include <algorithm>

namespace xr
{

//==============================================================================
IndexServer::IndexServer(Index capacity) noexcept
: mIds{ capacity == 0 ? nullptr :
    static_cast<Index*>(operator new(capacity * sizeof(Index), std::nothrow)) },
  mCapacity{ capacity }
{}

//==============================================================================
IndexServer::~IndexServer() noexcept
{
  operator delete(mIds);
}

//==============================================================================
IndexServer::Index  IndexServer::Acquire() noexcept
{
  uint32_t i = mLast;
  if (mReleased < i)
  {
    // grab the first known released id.
    i = mIds[mReleased];
  }
  else if (i < mCapacity)
  {
    // if we still have the capacity, create a new handle.
    mIds[mReleased] = i;
    ++mLast;
  }
  else
  {
    // we're out of indices
    return kInvalidIndex;
  }

  // if we were successful, we'll now shift the tracker up.
  ++mReleased;

  return i;
}

//==============================================================================
bool IndexServer::Release(Index i) noexcept
{
  auto iBegin = mIds;
  auto iEnd = iBegin + mReleased;
  auto iFind = std::find(iBegin, iEnd, i);  // TODO: assume LIFO, do reverse find?
  if (iFind == iEnd)
  {
    return false;
  }

  // Swap index with most recently activated one. Note that this makes our
  // registry of released indices like a stack - new releases always go
  // on the top.
  std::swap(*iFind, *(mIds + (mReleased - 1)));
  // Bump dealloc marker down, invalidating the last element (which is the one we've just released).
  --mReleased;

  return true;
}

//==============================================================================
IndexServer::Index IndexServer::GetNumActive() const noexcept
{
  return mLast;
}

//==============================================================================
IndexServer::Index IndexServer::GetNumAcquired() const noexcept
{
  return mReleased;
}

//==============================================================================
IndexServer::Index IndexServer::GetCapacity() const noexcept
{
  return mCapacity;
}

} // xr

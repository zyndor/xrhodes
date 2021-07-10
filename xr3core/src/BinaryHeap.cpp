//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/algorithm/BinaryHeap.hpp"

namespace xr
{
namespace detail
{

//==============================================================================
BinaryHeapCore::BinaryHeapCore(void* data, size_t capacity, size_t count)
: mData{ data },
  mCapacity{ capacity },
  mCount{ count }
{}

//==============================================================================
BinaryHeapCore::~BinaryHeapCore() = default;

//==============================================================================
size_t BinaryHeapCore::Capacity() const
{
  return mCapacity;
}

//==============================================================================
size_t BinaryHeapCore::Count() const
{
  return mCount;
}

//==============================================================================
bool BinaryHeapCore::IsEmpty() const
{
  return mCount == 0;
}

} // detail
} // xr

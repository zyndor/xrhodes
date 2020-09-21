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

//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/io/StructuredReader.hpp"

namespace xr
{
namespace Structured
{
namespace detail
{

void ReaderCore::AddRecord(Record&& record)
{
  auto result = mProperties.insert(record);
  if (result.second)
  {
    if (!mFirstRecord)
    {
      mFirstRecord = &*result.first;
    }
    else
    {
      mLastRecord->mNext = &*result.first;
    }
    mLastRecord = &*result.first;
  }
  else
  {
    throw std::runtime_error("Property key " + result.first->mKey + " already exists.");
  }
}

} // detail
} // StructuredFormat
} // xr

//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <algorithm>
#include <numeric>
#include "xr/UIID.hpp"
#include "xr/Timer.hpp"

namespace xr
{

const size_t kUIIDSize = 16;

//==============================================================================
UIID  UIID::Generate()
{
  UIID  uiid;

  UIID::iterator  iWrite(uiid.begin());
  UIID::iterator  iEnd(uiid.end());

  // append time in millisecs
  const uint64_t  tNow = Timer::GetUTC();

  std::copy(reinterpret_cast<const UIID::value_type*>(&tNow),
    reinterpret_cast<const UIID::value_type*>(&tNow) + sizeof(&tNow),
    iWrite);
  iWrite += sizeof(tNow); // 8 bytes

  // append address of local variable - random enough
  UIID::value_type* p = &uiid[0];
  auto pp = reinterpret_cast<UIID::value_type*>(&p);
  std::copy(pp, pp + sizeof(decltype(&p)), iWrite);
  iWrite += sizeof(&iWrite); // 4 or 8 bytes

  if(iWrite != iEnd)
  {
    // on <64 bit systems, jumble address and append again
    std::copy(pp, pp + sizeof(decltype(&p)), iWrite);
    //memcpy(iWrite, &pp, sizeof(&iWrite));
    std::reverse(iWrite, iWrite + sizeof(&iWrite));

    auto  sum = std::accumulate(iWrite, iWrite + sizeof(&iWrite), UIID::value_type(0),
      std::plus<UIID::value_type>());
    std::transform(iWrite, iWrite + sizeof(&iWrite), iWrite,
      [sum](UIID::value_type& v){ return v += sum; });

    iWrite += sizeof(&iWrite); // must be 4 bytes
  }

  // scramble digits
  UIID  temp(uiid);
  size_t  idx = 5;
  for(size_t i = 0; i < kSize; ++i)
  {
    uiid[i] = temp[idx];
    idx += 11;
    idx %= kSize;
  }

  return uiid;
}

} // xr

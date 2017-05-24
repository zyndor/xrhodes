//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <algorithm>
#include <numeric>
#include <XR/UIID.hpp>
#include <XR/Timer.hpp>

namespace XR
{

const size_t kUIIDSize = 16;

//==============================================================================
UIID  GenerateUIID()
{
  UIID  uiid;
  uiid.resize(kUIIDSize);

  UIID::iterator  iWrite(uiid.begin());
  UIID::iterator  iEnd(uiid.end());

  // append time in millisecs
  const uint64_t  tNow = Timer::GetUTC();

  const int kOffset = 0;
  std::copy(reinterpret_cast<const UIID::value_type*>(&tNow),
    reinterpret_cast<const UIID::value_type*>(&tNow + sizeof(&tNow)),
    iWrite);
  //memcpy(iWrite, &tNow + kOffset, sizeof(tNow) - kOffset);
  iWrite += sizeof(tNow) - kOffset; // 8 bytes

  // append address of local variable - random enough
  UIID::value_type*   p = &uiid[0];
  std::copy(reinterpret_cast<const UIID::value_type*>(&p),
    reinterpret_cast<const UIID::value_type*>(&p + sizeof(&p)),
    iWrite);
  //memcpy(iWrite, &pp, sizeof(&iWrite));
  iWrite += sizeof(&iWrite); // 4 or 8 bytes

  if(iWrite != iEnd)
  {
    // on <64 bit systems, jumble address and append again
    std::copy(reinterpret_cast<const UIID::value_type*>(&p),
      reinterpret_cast<const UIID::value_type*>(&p + sizeof(&p)),
      iWrite);
    //memcpy(iWrite, &pp, sizeof(&iWrite));
    std::reverse(iWrite, iWrite + sizeof(&iWrite));

    UIID::value_type  sum = std::accumulate(iWrite, iWrite + sizeof(&iWrite), 0);
    std::transform(iWrite, iWrite + sizeof(&iWrite), iWrite,
      [sum](UIID::value_type& v){ return v += sum; });

    iWrite += sizeof(&iWrite); // 4 or 8 bytes
  }

  // scramble digits
  UIID  temp(uiid);
  size_t  idx = 5;
  for(size_t i = 0; i < kUIIDSize; ++i)
  {
    uiid[i] = temp[idx];
    idx += 11;
    idx %= kUIIDSize;
  }

  return uiid;
}

} // XR
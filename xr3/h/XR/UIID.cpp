#include  <algorithm>
#include  <numeric>
#include  "UIID.hpp"
#include  "Timer.hpp"

namespace XR
{

const size_t kUIIDSize = 16;

//==============================================================================
UIID  GenerateUIID()
{
  UIID  uiid;
  uiid.resize(kUIIDSize);

  UIID::value_type*  pWrite(uiid.begin());
  UIID::value_type*  pEnd(uiid.end());

  // append time in millisecs
  const uint64  tNow = Timer::GetUTC();

  const int kOffset = 0;
  memcpy(pWrite, &tNow + kOffset, sizeof(tNow) - kOffset);
  pWrite += sizeof(tNow) - kOffset; // 8 bytes

  // append address of local variable - random enough
  UIID::value_type*   p = &uiid[0];
  UIID::value_type**  pp = &p;
  memcpy(pWrite, &pp, sizeof(&pWrite));
  pWrite += sizeof(&pWrite); // 4 or 8 bytes

  if(pWrite != pEnd)
  {
    // on <64 bit systems, jumble address and append again
    memcpy(pWrite, &pp, sizeof(&pWrite));
    std::reverse(pWrite, pWrite + sizeof(&pWrite));
    UIID::value_type  sum = std::accumulate(pWrite, pWrite + sizeof(&pWrite), 0);
    std::transform(pWrite, pWrite + sizeof(&pWrite), pWrite,
      std::bind2nd(std::plus<UIID::value_type>(), sum));

    pWrite += sizeof(&pWrite); // 4 or 8 bytes
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
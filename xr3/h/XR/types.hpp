//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_TYPES_HPP
#define XR_TYPES_HPP

#include <cstdint>
#include <limits>

namespace XR
{

//==============================================================================
int16_t ClipToInt16(int32_t val);

//==============================================================================
// implementation
//==============================================================================
inline
int16_t ClipToInt16(int32_t val)
{
  if (((val - std::numeric_limits<int16_t>::min()) &
    ~std::numeric_limits<uint16_t>::max()) != 0)
  {
    bool  over(val > std::numeric_limits<int16_t>::max());
    bool  under(val < std::numeric_limits<int16_t>::min());
    XR_ASSERT(ClipToInt16, over ^ under);
    val = over * std::numeric_limits<int16_t>::max() + under *
      std::numeric_limits<int16_t>::min();
  }

  return static_cast<int16_t>(val);
}

} // XR

#endif // XR_TYPES_HPP
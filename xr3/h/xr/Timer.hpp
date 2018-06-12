#ifndef XR_TIMER_HPP
#define XR_TIMER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "xr/fundamentals.hpp"

namespace xr
{

//==============================================================================
class Timer
{
  XR_NONOBJECT_DECL(Timer)

public:
  ///@return Number of milliseconds since the epoch (1st January 1970 00:00)
  static uint64_t GetUTC();

  ///@return Uninterrupted system time in milliseconds. Unaffected by clock
  /// adjustments.
  static uint64_t GetUST();

  ///@return Uninterrupted system time in nanoseconds. Unaffected by clock
  /// adjustments.
  static uint64_t GetUSTNano();
};

} // xr

#endif // XR_TIMER_HPP

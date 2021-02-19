#ifndef XR_TIMER_HPP
#define XR_TIMER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "xr/types/fundamentals.hpp"

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
  static double GetUST();

  ///@return Uninterrupted system time in nanoseconds. Unaffected by clock
  /// adjustments.
  static double GetUSTNano();
};

} // xr

#endif // XR_TIMER_HPP

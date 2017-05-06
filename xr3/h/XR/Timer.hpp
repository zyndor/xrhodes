//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_TIMER_HPP
#define XR_TIMER_HPP

#include <XR/fundamentals.hpp>

namespace XR
{

//==============================================================================
class Timer
{
  XR_NONOBJECT_DECL(Timer)

public:
  // general
  static void Init();
  static void Exit();

  static uint64_t GetUTC();
  static uint64_t GetUST();
  static uint64_t GetUSTNano();
};

} // XR

#endif // XR_TIMER_HPP
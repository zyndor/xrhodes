//
// Nuclear Heart Games
// XRhodes
//
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
//
// copyright (c) 2011 - 2016. All rights reserved.
//
//==============================================================================
#ifndef XR_TIMER_HPP
#define XR_TIMER_HPP

#include "types.hpp"

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

  static uint64 GetUTC();
  static uint64 GetUST();
  static uint64 GetUSTNano();
};

} // XR

#endif // XR_TIMER_HPP
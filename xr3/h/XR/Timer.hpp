//
// Nuclear Heart Games
// XRhodes
//
// Timer.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    11/09/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_TIMER_HPP
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
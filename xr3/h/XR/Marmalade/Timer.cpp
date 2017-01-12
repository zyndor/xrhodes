//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Timer.hpp"
#include <s3eTimer.h>

namespace XR
{

void  Timer::Init()
{}

void  Timer::Exit()
{}

uint64 Timer::GetUTC()
{
  return s3eTimerGetUTC();
}

uint64 Timer::GetUST()
{
  return s3eTimerGetUST();
}

uint64 Timer::GetUSTNano()
{
  return s3eTimerGetUSTNanoseconds();
}

} // XR
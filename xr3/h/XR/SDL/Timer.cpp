//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <SDL_timer.h>
#include <XR/Timer.hpp>

namespace XR
{

void  Timer::Init()
{}

void  Timer::Exit()
{}

uint64 Timer::GetUTC()
{
  return 0;
}

uint64 Timer::GetUST()
{
  return SDL_GetTicks();
}

uint64 Timer::GetUSTNano()
{
  return GetUST() * uint64(1000);
}

} // XR
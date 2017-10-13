//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "SDL_timer.h"
#include "XR/Timer.hpp"

namespace XR
{

void  Timer::Init()
{}

void  Timer::Exit()
{}

uint64_t Timer::GetUTC()
{
  return 0;
}

uint64_t Timer::GetUST()
{
  return SDL_GetTicks();
}

uint64_t Timer::GetUSTNano()
{
  return GetUST() * uint64_t(1000);
}

} // XR
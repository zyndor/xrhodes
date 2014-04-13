#include <SDL2/SDL_timer.h>
#include "Timer.hpp"
#include "Callback.hpp"

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
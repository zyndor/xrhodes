#include <s3eTimer.h>
#include "Timer.hpp"

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
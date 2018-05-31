//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Timer.hpp"
#include <chrono>
#include <ctime>

namespace sc = std::chrono;

namespace XR
{
namespace
{
const auto s_ust0 = sc::steady_clock::now();
}

uint64_t Timer::GetUTC()
{
  auto utc = sc::system_clock::now().time_since_epoch();
  auto ms = sc::duration_cast<sc::milliseconds>(utc);
  return ms.count();
}

uint64_t Timer::GetUST()
{
  auto ust = sc::steady_clock::now() - s_ust0;
  return sc::duration_cast<sc::milliseconds>(ust).count();
}

uint64_t Timer::GetUSTNano()
{
  auto ust = sc::steady_clock::now() - s_ust0;
  return sc::duration_cast<sc::nanoseconds>(ust).count();
}

} // XR

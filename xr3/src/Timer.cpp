//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Timer.hpp"
#include <chrono>
#include <ctime>

namespace sc = std::chrono;

namespace xr
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

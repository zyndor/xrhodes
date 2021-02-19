//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/memory/ScopeGuard.hpp"
#include <list>

using namespace xr;

namespace
{

XM_TEST(ScopeGuard, Guard)
{
  std::list<int>  ints{ 0, 1, 2 };

  try
  {
    ints.push_back(3);
    auto intsPushGuard = MakeScopeGuard([&ints]()
    {
      ints.pop_back();
    });

    throw std::logic_error("XM_TEST logic error.");
  }
  catch (...)
  {
  }

  bool cool = ints == decltype(ints){ 0, 1, 2 };
  XM_ASSERT_TRUE(cool);
}

XM_TEST(ScopeGuard, Release)
{
  std::list<int>  ints{ 0, 1, 2 };

  try
  {
    ints.push_back(3);
    auto intsPushGuard = MakeScopeGuard([&ints]()
    {
      ints.pop_back();
    });

    intsPushGuard.Release();
    throw std::logic_error("XM_TEST logic error.");
  }
  catch (...)
  {
  }

  bool cool = ints == decltype(ints){ 0, 1, 2, 3 };
  XM_ASSERT_TRUE(cool);
}

XM_TEST(ScopeGuard, Transfer)
{
  std::list<int>  ints{ 0, 1, 2 };

  try
  {
    ints.push_back(3);
    auto intsPushGuard = MakeScopeGuard([&ints]()
    {
      ints.pop_back();
    });

    auto otherGuard = intsPushGuard;

    intsPushGuard.Release();
    throw std::logic_error("XM_TEST logic error.");
  }
  catch (...)
  {
  }

  bool cool = ints == decltype(ints){ 0, 1, 2 };
  XM_ASSERT_TRUE(cool);
}

}


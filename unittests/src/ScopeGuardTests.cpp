#include "gtest/gtest.h"
#include "xr/ScopeGuard.hpp"
#include <list>

namespace xr
{
  TEST(ScopeGuard, Guard)
  {
    std::list<int>  ints{ 0, 1, 2 };

    try
    {
      ints.push_back(3);
      auto intsPushGuard = MakeScopeGuard([&ints]()
      {
        ints.pop_back();
      });

      throw std::logic_error("Test logic error.");
    }
    catch (...)
    {
    }

    bool cool = ints == decltype(ints){ 0, 1, 2 };
    ASSERT_TRUE(cool);
  }

  TEST(ScopeGuard, Release)
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
      throw std::logic_error("Test logic error.");
    }
    catch (...)
    {
    }

    bool cool = ints == decltype(ints){ 0, 1, 2, 3 };
    ASSERT_TRUE(cool);
  }

  TEST(ScopeGuard, Transfer)
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
      throw std::logic_error("Test logic error.");
    }
    catch (...)
    {
    }

    bool cool = ints == decltype(ints){ 0, 1, 2 };
    ASSERT_TRUE(cool);
  }
}

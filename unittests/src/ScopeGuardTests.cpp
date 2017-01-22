#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/ScopeGuard.hpp>
#include <list>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(ScopeGuardTests)
  {
  public:

    TEST_METHOD(ScopeGuard_Guard)
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

      Assert::IsTrue(ints == decltype(ints){ 0, 1, 2 });
    }

    TEST_METHOD(ScopeGuard_Release)
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

      Assert::IsTrue(ints == decltype(ints){ 0, 1, 2, 3 });
    }

    TEST_METHOD(ScopeGuard_Transfer)
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

      Assert::IsTrue(ints == decltype(ints){ 0, 1, 2 });
    }
  };
}
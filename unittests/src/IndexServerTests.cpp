#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/IndexServer.hpp>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
TEST_CLASS(IndexServerTests)
{
  TEST_METHOD(IndexServer_Basics)
  {
    IndexServer serv(4);

    // acquiring should give you indices in ascending order
    Assert::IsTrue(serv.Acquire() == 0);
    Assert::IsTrue(serv.Acquire() == 1);
    Assert::IsTrue(serv.Acquire() == 2);
    Assert::IsTrue(serv.Acquire() == 3);

    // until the capacity has depleted - should fail with an exception
    try
    {
      auto dummy = serv.Acquire();
      Assert::Fail(L"Should have thrown");
    }
    catch (std::bad_alloc&)
    {
    }
    catch (...)
    {
      Assert::Fail(L"Wrong exception type");
    }

    serv.Release(0);
    serv.Release(3);
    serv.Release(1);
    serv.Release(2);
    // after releasing, the first one we will acquire is the least recently released
    Assert::IsTrue(serv.Acquire() == 2);
    Assert::IsTrue(serv.Acquire() == 1);
    Assert::IsTrue(serv.Acquire() == 3);
    Assert::IsTrue(serv.Acquire() == 0);

    serv.Release(2);
    // duplicate releases should fail with an exceptions
    try
    {
      serv.Release(2);
      Assert::Fail(L"Should have thrown");
    }
    catch (std::logic_error&)
    {}
    catch (...)
    {
      Assert::Fail(L"Wrong exception type");
    }
  }
};
}

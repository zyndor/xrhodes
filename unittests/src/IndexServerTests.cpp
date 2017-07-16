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
    Assert::IsTrue(serv.GetNumActive() == 0);
    Assert::IsTrue(serv.GetNumAcquired() == 0);
    Assert::IsTrue(serv.Acquire() == 0);
    Assert::IsTrue(serv.GetNumActive() == 1);
    Assert::IsTrue(serv.GetNumAcquired() == 1);
    Assert::IsTrue(serv.Acquire() == 1);
    Assert::IsTrue(serv.GetNumActive() == 2);
    Assert::IsTrue(serv.GetNumAcquired() == 2);
    Assert::IsTrue(serv.Acquire() == 2);
    Assert::IsTrue(serv.GetNumActive() == 3);
    Assert::IsTrue(serv.GetNumAcquired() == 3);
    Assert::IsTrue(serv.Acquire() == 3);
    Assert::IsTrue(serv.GetNumActive() == 4); // increasing with each allocation.
    Assert::IsTrue(serv.GetNumAcquired() == 4); // increasing with each allocation.

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
    Assert::IsTrue(serv.GetNumActive() == 4); // never decreasing
    Assert::IsTrue(serv.GetNumAcquired() == 3); // decreasing with releases
    serv.Release(3);
    Assert::IsTrue(serv.GetNumActive() == 4); // never decreasing
    Assert::IsTrue(serv.GetNumAcquired() == 2); // decreasing with releases
    serv.Release(1);
    Assert::IsTrue(serv.GetNumActive() == 4); // never decreasing
    Assert::IsTrue(serv.GetNumAcquired() == 1); // decreasing with releases
    serv.Release(2);
    Assert::IsTrue(serv.GetNumActive() == 4); // never decreasing
    Assert::IsTrue(serv.GetNumAcquired() == 0); // decreasing with releases
    // after releasing, the first one we will acquire is the least recently released
    Assert::IsTrue(serv.Acquire() == 2);
    Assert::IsTrue(serv.Acquire() == 1);
    Assert::IsTrue(serv.Acquire() == 3);
    Assert::IsTrue(serv.Acquire() == 0);
    Assert::IsTrue(serv.GetNumActive() == 4); // no change
    Assert::IsTrue(serv.GetNumAcquired() == 4); // and back up with allocations

    serv.Release(2);
    try
    {
      // duplicate releases should fail with an exceptions
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

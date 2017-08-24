#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/Counted.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{

TEST_CLASS(CountedTests)
{
  struct NoopDeleter
  {
    void operator()(void*)
    {}
  };

  struct MockCountable: Countable
  {
    void Acquire()
    {
      ++count;
    }

    bool Release()
    {
      bool success = count == 1;
      --count;
      return success;
    }

    int count = 0;
  };

  using Counted = Counted<MockCountable, NoopDeleter>;

  MockCountable o;

  TEST_METHOD(Counted_CtorResetDtor)
  {
    {
      Counted c(&o);
      Assert::IsTrue(o.count == 1);
      Assert::IsTrue(c);

      c.Reset(&o);
      Assert::IsTrue(o.count == 1);

      c.Reset(nullptr);
      Assert::IsTrue(o.count == 0);
      Assert::IsTrue(!c);

      c.Reset(&o);
    }

    Assert::IsTrue(o.count == 0);
  }

  TEST_METHOD(Counted_Copy)
  {
    Counted c(&o);
    Counted c2(c);

    Assert::IsTrue(o.count == 2);
    Assert::IsTrue(c);
    Assert::IsTrue(c2);
  }

  TEST_METHOD(Counted_Move)
  {
    Counted c(&o);
    Counted c2(std::move(c));

    Assert::IsTrue(o.count == 1);
    Assert::IsTrue(!c);
    Assert::IsTrue(c2);
  }
};

}

//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/memory/Counted.hpp"

namespace xr
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

  using MockCounted = Counted<MockCountable>;

  MockCountable o;

  TEST(Counted, CtorResetDtor)
  {
    {
      MockCounted c(&o, NoopDeleter());
      ASSERT_EQ(o.count, 1);
      ASSERT_NE(c.Get(), nullptr);

      c.Reset(&o);
      ASSERT_EQ(o.count, 1);

      c.Reset(nullptr);
      ASSERT_EQ(o.count, 0);
      ASSERT_EQ(c.Get(), nullptr);

      c.Reset(&o);
    }

    ASSERT_EQ(o.count, 0);
  }

  TEST(Counted, Copy)
  {
    MockCounted c(&o, NoopDeleter());
    MockCounted c2(c);

    ASSERT_EQ(o.count, 2);
    ASSERT_NE(c.Get(), nullptr);
    ASSERT_NE(c2.Get(), nullptr);
  }

  TEST(Counted, Move)
  {
    MockCounted c(&o, NoopDeleter());
    MockCounted c2(std::move(c));

    ASSERT_EQ(o.count, 1);
    ASSERT_EQ(c.Get(), nullptr);
    ASSERT_NE(c2.Get(), nullptr);
  }

}

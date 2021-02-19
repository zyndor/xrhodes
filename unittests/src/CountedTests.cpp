//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/memory/Counted.hpp"

using namespace xr;

namespace
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

XM_TEST(Counted, CtorResetDtor)
{
  {
    MockCounted c(&o, NoopDeleter());
    XM_ASSERT_EQ(o.count, 1);
    XM_ASSERT_NE(c.Get(), nullptr);

    c.Reset(&o);
    XM_ASSERT_EQ(o.count, 1);

    c.Reset(nullptr);
    XM_ASSERT_EQ(o.count, 0);
    XM_ASSERT_EQ(c.Get(), nullptr);

    c.Reset(&o);
  }

  XM_ASSERT_EQ(o.count, 0);
}

XM_TEST(Counted, Copy)
{
  MockCounted c(&o, NoopDeleter());
  MockCounted c2(c);

  XM_ASSERT_EQ(o.count, 2);
  XM_ASSERT_NE(c.Get(), nullptr);
  XM_ASSERT_NE(c2.Get(), nullptr);
}

XM_TEST(Counted, Move)
{
  MockCounted c(&o, NoopDeleter());
  MockCounted c2(std::move(c));

  XM_ASSERT_EQ(o.count, 1);
  XM_ASSERT_EQ(c.Get(), nullptr);
  XM_ASSERT_NE(c2.Get(), nullptr);
}

}

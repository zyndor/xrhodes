//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/Rect.hpp"

namespace xr
{

void AssertEqual(Rect const& lhs, Rect const& rhs)
{
  ASSERT_EQ(lhs.x, rhs.x);
  ASSERT_EQ(lhs.y, rhs.y);
  ASSERT_EQ(lhs.w, rhs.w);
  ASSERT_EQ(lhs.h, rhs.h);
}

TEST(Rect, BasicOperations)
{
  Rect r0(-50, -50, 100, 100);
  Rect r1(0, 0, 100, 20);

  ASSERT_TRUE(!r0.IsZero());

  Rect u = r0.Union(r1);
  AssertEqual(u, Rect(-50, -50, 150, 100));
  ASSERT_FALSE(u.IsZero());

  Rect i = r0.Intersection(r1);
  AssertEqual(i, Rect(0, 0, 50, 20));
  ASSERT_FALSE(i.IsZero());

  r0 = Rect(100, 100, 20, 20);
  i = r0.Intersection(r1);
  ASSERT_TRUE(i.IsZero());
}

}

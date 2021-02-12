//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/math/Rect.hpp"

using namespace xr;

namespace
{

void AssertEqual(Rect const& lhs, Rect const& rhs)
{
  XM_ASSERT_EQ(lhs.x, rhs.x);
  XM_ASSERT_EQ(lhs.y, rhs.y);
  XM_ASSERT_EQ(lhs.w, rhs.w);
  XM_ASSERT_EQ(lhs.h, rhs.h);
}

XM_TEST(Rect, BasicOperations)
{
  Rect r0(-50, -50, 100, 100);
  Rect r1(0, 0, 100, 20);

  XM_ASSERT_TRUE(!r0.IsZero());

  Rect u = r0.Union(r1);
  AssertEqual(u, Rect(-50, -50, 150, 100));
  XM_ASSERT_FALSE(u.IsZero());

  Rect i = r0.Intersection(r1);
  AssertEqual(i, Rect(0, 0, 50, 20));
  XM_ASSERT_FALSE(i.IsZero());

  r0 = Rect(100, 100, 20, 20);
  i = r0.Intersection(r1);
  XM_ASSERT_TRUE(i.IsZero());
}

}

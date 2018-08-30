//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/Queue.hpp"

namespace xr
{
	TEST(Queue, BasicOperations)
	{
    // default construction
    Queue<int>  q;
    ASSERT_TRUE(q.empty());
    ASSERT_EQ(q.size(), 0U);

    // push_back()
    for (int i = 0; i < 100; ++i)
    {
      q.push_back(i);
    }

    ASSERT_TRUE(!q.empty());
    ASSERT_EQ(q.size(), 100U);

    // remove()
    q.remove(45);
    ASSERT_EQ(q.size(), 99U);

    // traversal and push / remove operations
    auto i = q.begin();
    for (int j = 0; j < 45; ++j)
    {
      ASSERT_EQ(*i, j);
      ++i;
    }

    for (int j = 46; j < 100; ++j)
    {
      ASSERT_EQ(*i, j);
      ++i;
    }

    // front() and pop_front()
    ASSERT_EQ(q.front(), 0);
    q.pop_front();
    ASSERT_EQ(q.size(), 98U);
    ASSERT_EQ(q.front(), 1);

    // copy construction
    auto q2 = q;
    ASSERT_EQ(q.size(), 98U);
    ASSERT_EQ(q2.size(), 98U);

    // assignment
    q = q2;
    ASSERT_EQ(q.size(), 98U);
    ASSERT_EQ(q2.size(), 98U);

    i = q.begin();
    for (auto j : q2)
    {
      ASSERT_EQ(j, *i);
      ++i;
    }

    // adopt() -- note that remove() is also implemented based on adopt().
    q.adopt(q2);
    ASSERT_EQ(q.size(), 196U);
    ASSERT_TRUE(q2.empty());

    // clear()
    q.clear();
    ASSERT_TRUE(q.empty());
    ASSERT_EQ(q.size(), 0U);
  }
}

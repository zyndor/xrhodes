//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/memory/Queue.hpp"

using namespace xr;

namespace
{

XM_TEST(Queue, BasicOperations)
{
  // default construction
  Queue<int>  q;
  XM_ASSERT_TRUE(q.empty());
  XM_ASSERT_EQ(q.size(), 0U);

  // push_back()
  for (int i = 0; i < 100; ++i)
  {
    q.push_back(i);
  }

  XM_ASSERT_TRUE(!q.empty());
  XM_ASSERT_EQ(q.size(), 100U);

  // remove()
  q.remove(45);
  XM_ASSERT_EQ(q.size(), 99U);

  // traversal and push / remove operations
  auto i = q.begin();
  for (int j = 0; j < 45; ++j)
  {
    XM_ASSERT_EQ(*i, j);
    ++i;
  }

  for (int j = 46; j < 100; ++j)
  {
    XM_ASSERT_EQ(*i, j);
    ++i;
  }

  // front() and pop_front()
  XM_ASSERT_EQ(q.front(), 0);
  q.pop_front();
  XM_ASSERT_EQ(q.size(), 98U);
  XM_ASSERT_EQ(q.front(), 1);

  // copy construction
  auto q2 = q;
  XM_ASSERT_EQ(q.size(), 98U);
  XM_ASSERT_EQ(q2.size(), 98U);

  // assignment
  q = q2;
  XM_ASSERT_EQ(q.size(), 98U);
  XM_ASSERT_EQ(q2.size(), 98U);

  i = q.begin();
  for (auto j : q2)
  {
    XM_ASSERT_EQ(j, *i);
    ++i;
  }

  // adopt() -- note that remove() is also implemented based on adopt().
  q.adopt(q2);
  XM_ASSERT_EQ(q.size(), 196U);
  XM_ASSERT_TRUE(q2.empty());

  // clear()
  q.clear();
  XM_ASSERT_TRUE(q.empty());
  XM_ASSERT_EQ(q.size(), 0U);
}

}

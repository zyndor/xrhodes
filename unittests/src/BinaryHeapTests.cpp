//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/algorithm/BinaryHeap.hpp"
#include <functional>

namespace
{

template <template <typename> class Comp>
void TestHeap(xr::BinaryHeap<char, Comp>& heap)
{
  ASSERT_TRUE(heap.IsEmpty());
  ASSERT_EQ(heap.Count(), 0);

  size_t count = 0;
  for (char i : "Hello world and thanks for all the fish!<3(>3")
  {
    heap.Push(i);
    ASSERT_GE(heap.Capacity(), heap.Count());
    ASSERT_FALSE(heap.IsEmpty());

    ++count;
    ASSERT_EQ(heap.Count(), count);
  }

  heap.Shrink();
  ASSERT_EQ(heap.Count(), heap.Capacity());
}

TEST(BinaryHeap, MaxHeap)
{
  xr::BinaryHeap<char, std::less> heap;
  TestHeap(heap);

  size_t count = heap.Count();
  char  max = std::numeric_limits<char>::max();
  while (!heap.IsEmpty())
  {
    auto peek = heap.Peek();
    ASSERT_GE(max, peek);
    max = heap.Pop();
    printf("%c", max);
    ASSERT_EQ(peek, max);
    ASSERT_GE(heap.Capacity(), heap.Count());

    --count;
    ASSERT_EQ(count, heap.Count());
  }
}

TEST(BinaryHeap, MinHeap)
{
  xr::BinaryHeap<char, std::greater> heap;
  TestHeap(heap);

  size_t count = heap.Count();
  int32_t min = std::numeric_limits<char>::min();
  while (!heap.IsEmpty())
  {
    auto peek = heap.Peek();
    ASSERT_LE(min, peek);
    min = heap.Pop();
    printf("%c", min);
    ASSERT_EQ(peek, min);
    ASSERT_GE(heap.Capacity(), heap.Count());

    --count;
    ASSERT_EQ(count, heap.Count());
  }
}

}

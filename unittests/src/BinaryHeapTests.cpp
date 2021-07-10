//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/algorithm/BinaryHeap.hpp"
#include <functional>

namespace
{

template <template <typename> class Comp>
void TestHeap(xr::BinaryHeap<char, Comp<char>>& heap)
{
  XM_ASSERT_TRUE(heap.IsEmpty());
  XM_ASSERT_EQ(heap.Count(), size_t(0));

  size_t count = 0;
  for (char i : "Hello world and thanks for all the fish!<3(>3")
  {
    heap.Push(i);
    XM_ASSERT_GE(heap.Capacity(), heap.Count());
    XM_ASSERT_FALSE(heap.IsEmpty());

    ++count;
    XM_ASSERT_EQ(heap.Count(), count);
  }

  heap.Shrink();
  XM_ASSERT_EQ(heap.Count(), heap.Capacity());
}

XM_TEST(BinaryHeap, MaxHeap)
{
  xr::BinaryHeap<char, std::less<char>> heap;
  TestHeap(heap);

  size_t count = heap.Count();
  char  max = std::numeric_limits<char>::max();
  while (!heap.IsEmpty())
  {
    auto peek = heap.Peek();
    XM_ASSERT_GE(max, peek);
    max = heap.Pop();
    printf("%c", max);
    XM_ASSERT_EQ(peek, max);
    XM_ASSERT_GE(heap.Capacity(), heap.Count());

    --count;
    XM_ASSERT_EQ(count, heap.Count());
  }
}

XM_TEST(BinaryHeap, MinHeap)
{
  xr::BinaryHeap<char, std::greater<char>> heap;
  TestHeap(heap);

  size_t count = heap.Count();
  int32_t min = std::numeric_limits<char>::min();
  while (!heap.IsEmpty())
  {
    auto peek = heap.Peek();
    XM_ASSERT_LE(min, peek);
    min = heap.Pop();
    printf("%c", min);
    XM_ASSERT_EQ(peek, min);
    XM_ASSERT_GE(heap.Capacity(), heap.Count());

    --count;
    XM_ASSERT_EQ(count, heap.Count());
  }
}

}

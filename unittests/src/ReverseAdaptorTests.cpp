//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/types/ReverseAdaptor.hpp"
#include <vector>

using namespace xr;

namespace
{

template <typename T>
void TestReverseAdaptor(T& data)
{
  typename std::decay<decltype(data[0])>::type max = 0;
  for (auto i : MakeReverseAdaptor(data))
  {
    XM_ASSERT_GT(i, max);
    max = i;
  }
}

XM_TEST(ReverseAdaptor, Container)
{
  std::vector<uint32_t> data{ 100, 10, 1 };
  TestReverseAdaptor(data);
}

XM_TEST(ReverseAdaptor, Array)
{
  uint32_t data[] = { 100, 10, 1 };
  TestReverseAdaptor(data);
}

}

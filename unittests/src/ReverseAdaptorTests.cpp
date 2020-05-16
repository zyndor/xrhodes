//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/types/ReverseAdaptor.hpp"

using namespace xr;

namespace
{

struct ReverseAdaptor: public testing::Test
{
  static void SetUpTestCase()
  {}

  static void TearDownTestCase()
  {}

  template <typename T>
  void TestReverseAdaptor(T& data)
  {
    typename std::decay<decltype(data[0])>::type max = 0;
    for (auto i : MakeReverseAdaptor(data))
    {
      ASSERT_GT(i, max);
      max = i;
    }
  }
};

TEST_F(ReverseAdaptor, Container)
{
  std::vector<uint32_t> data{ 100, 10, 1 };
  TestReverseAdaptor(data);
}

TEST_F(ReverseAdaptor, Array)
{
  uint32_t data[] = { 100, 10, 1 };
  TestReverseAdaptor(data);
}

}

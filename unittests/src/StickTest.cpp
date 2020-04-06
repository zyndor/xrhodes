//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/strings/Stick.hpp"

using namespace xr;

namespace
{

TEST(Stick, Basic)
{
  const Stick stk("hello world!");
  ASSERT_EQ(stk.mSize, 12);
  ASSERT_EQ(strcmp(stk, "hello world!"), 0);

  uint32_t i = 0;
  for (auto& s : stk)
  {
    ++i;
  }
  ASSERT_EQ(i, stk.mSize);
}

TEST(Stick, Empty)
{
  const Stick stk("");
  ASSERT_EQ(stk.mSize, 0);
  ASSERT_EQ(strcmp(stk, ""), 0);

  uint32_t i = 0;
  for (auto& s : stk)
  {
    ++i;
  }
  ASSERT_EQ(i, stk.mSize);
}

}

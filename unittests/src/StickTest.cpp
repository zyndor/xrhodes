//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/strings/Stick.hpp"

using namespace xr;

namespace
{

XM_TEST(Stick, Basic)
{
  const Stick stk("hello world!");
  XM_ASSERT_EQ(stk.mSize, 12);
  XM_ASSERT_EQ(strcmp(stk, "hello world!"), 0);

  uint32_t i = 0;
  for (auto& s : stk)
  {
    ++i;
  }
  XM_ASSERT_EQ(i, stk.mSize);
}

XM_TEST(Stick, Empty)
{
  const Stick stk("");
  XM_ASSERT_EQ(stk.mSize, 0);
  XM_ASSERT_EQ(strcmp(stk, ""), 0);

  uint32_t i = 0;
  for (auto& s : stk)
  {
    ++i;
  }
  XM_ASSERT_EQ(i, stk.mSize);
}

}

//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/strings/HardString.hpp"

using namespace xr;

namespace
{

using TestString = HardString<65536>;

XM_TEST(HardString, Basics)
{
  TestString  str;
  XM_ASSERT_EQ(str, "");
  XM_ASSERT_EQ(str.size(), 0u);

  str = "abcd";
  XM_ASSERT_EQ(str, "abcd");
  XM_ASSERT_EQ(str.size(), 4u);

  str = TestString(123);
  XM_ASSERT_EQ(str, "123");
  XM_ASSERT_EQ(str.size(), 3u);

  str += 456;
  XM_ASSERT_EQ(str, "123456");
  XM_ASSERT_EQ(str.size(), 6u);

  {
    auto p = str.find("34");
    XM_ASSERT_EQ(p, str.c_str() + 2);
  }

  {
    auto p = str.find("35");
    XM_ASSERT_EQ(p, nullptr);
  }

  {
    auto p = str.find('4');
    XM_ASSERT_EQ(p, str.c_str() + 3);
  }

  str += 456;
  XM_ASSERT_EQ(str.size(), 9u);

  {
    auto p = str.rfind("45");
    XM_ASSERT_EQ(p, str.c_str() + 6);
  }

  {
    auto p = str.rfind("46");
    XM_ASSERT_EQ(p, nullptr);
  }

  {
    auto p = str.rfind('4');
    XM_ASSERT_EQ(p, str.c_str() + 6);
  }

  {
    auto str2 = str.substr(4, 4);
    XM_ASSERT_EQ(str2, "5645");
    XM_ASSERT_EQ(str2.size(), 4u);
  }

  // and finally
  str.clear();
  XM_ASSERT_EQ(str, "");
  XM_ASSERT_EQ(str.size(), 0u);
}

}

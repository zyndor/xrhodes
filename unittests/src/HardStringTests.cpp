#include <gtest/gtest.h>
#include "XR/HardString.hpp"

namespace XR
{
  using TestString = HardString<65536>;

  TEST(HardString, Basics)
  {
    TestString  str;
    ASSERT_EQ(str, "");
    ASSERT_EQ(str.size(), 0);

    str = "abcd";
    ASSERT_EQ(str, "abcd");
    ASSERT_EQ(str.size(), 4);

    str = TestString(123);
    ASSERT_EQ(str, "123");
    ASSERT_EQ(str.size(), 3);

    str += 456;
    ASSERT_EQ(str, "123456");
    ASSERT_EQ(str.size(), 6);

    {
      auto p = str.find("34");
      ASSERT_EQ(p, str.c_str() + 2);
    }

    {
      auto p = str.find("35");
      ASSERT_EQ(p, nullptr);
    }

    {
      auto p = str.find('4');
      ASSERT_EQ(p, str.c_str() + 3);
    }

    str += 456;
    ASSERT_EQ(str.size(), 9);

    {
      auto p = str.rfind("45");
      ASSERT_EQ(p, str.c_str() + 6);
    }

    {
      auto p = str.rfind("46");
      ASSERT_EQ(p, nullptr);
    }

    {
      auto p = str.rfind('4');
      ASSERT_EQ(p, str.c_str() + 6);
    }

    {
      auto str2 = str.substr(4, 4);
      ASSERT_EQ(str2, "5645");
      ASSERT_EQ(str2.size(), 4);
    }

    // and finally
    str.clear();
    ASSERT_EQ(str, "");
    ASSERT_EQ(str.size(), 0);
  }
}

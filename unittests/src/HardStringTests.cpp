#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/HardString.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(HardStringTests)
  {
  public:

    using TestString = HardString<65536>;

    TEST_METHOD(HardString_Basics)
    {
      TestString  str;
      Assert::IsTrue(str == "");

      str = TestString(123);
      Assert::IsTrue(str == "123");
      Assert::IsTrue(str.size() == 3);

      str += 456;
      Assert::IsTrue(str == "123456");
      Assert::IsTrue(str.size() == 6);

      {
        auto p = str.find("34");
        Assert::IsTrue(p == str.c_str() + 2);
      }

      {
        auto p = str.find("35");
        Assert::IsTrue(p == nullptr);
      }

      {
        auto p = str.find('4');
        Assert::IsTrue(p == str.c_str() + 3);
      }

      str += 456;
      Assert::IsTrue(str.size() == 9);

      {
        auto p = str.rfind("45");
        Assert::IsTrue(p == str.c_str() + 6);
      }

      {
        auto p = str.rfind("46");
        Assert::IsTrue(p == nullptr);
      }

      {
        auto p = str.rfind('4');
        Assert::IsTrue(p == str.c_str() + 6);
      }

      {
        auto str2 = str.substr(4, 4);
        Assert::IsTrue(str2 == "5645");
        Assert::IsTrue(str2.size() == 4);
      }
    }
  };
}
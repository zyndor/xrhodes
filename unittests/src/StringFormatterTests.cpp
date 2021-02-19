//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/strings/StringFormatter.hpp"

using namespace xr;

namespace
{
namespace Test
{
  struct Rect
  {
    int width, height;
  };

  std::ostream& operator<<(std::ostream& os, Rect const& r)
  {
    return os << "{" << r.width << " x " << r.height << "}";
  }
}

XM_TEST(StringFormatter, Basics)
{
  StringFormatter sfm("Hello %1, would you like your Rect to be %2? %5, %4, %3...");
  sfm % "Bob" % ::Test::Rect{ 10, 30 } % 1 % 2.0f % '3';

  XM_ASSERT_EQ(sfm.GetString(), "Hello Bob, would you like your Rect to be {10 x 30}? 3, 2, 1...");
  XM_ASSERT_EQ(sfm.GetString(), "Hello Bob, would you like your Rect to be {10 x 30}? 3, 2, 1..."); // GetString() doesn't consume the string.
}

XM_TEST(StringFormatter, Extremes) // Strings shorter than a substitutor, including empty strings must work.
{
  StringFormatter sfm("%1%1D%4s%1%3xyr%2%1b%3nuc%1l%4%2c%1%1");
  sfm % "" % "i" % "o" % "e";

  XM_ASSERT_EQ(sfm.GetString(), "Desoxyribonucleic");
}

XM_TEST(StringFormatter, GenerateToken)
{
  StringFormatter sfm("%1");
  sfm % "Good %2" % "morning %3" % "Mister %4" % "Sunshine%9 %5" % "It's %6" % "%8 %7" % "fine day%9" % "a mighty";
  sfm % "! %1 %2 %3 %4 %5 %6 %7 %8 %9"; // Tokens generated after substitution (including current one - %9) don't get substituted again

  // %1
  // Good %2
  // Good morning %3
  // Good morning Mister %4
  // Good morning Mister Sunshine%9 %5
  // Good morning Mister Sunshine%9 It's %6
  // Good morning Mister Sunshine%9 It's %8 %7
  // Good morning Mister Sunshine%9 It's %8 fine day%9
  // Good morning Mister Sunshine%9 It's a mighty fine day%9
  XM_ASSERT_EQ(sfm.GetString(), "Good morning Mister Sunshine! %1 %2 %3 %4 %5 %6 %7 %8 %9 It's a mighty fine day! %1 %2 %3 %4 %5 %6 %7 %8 %9");
}

}

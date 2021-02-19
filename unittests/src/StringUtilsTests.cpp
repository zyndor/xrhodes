//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/strings/stringutils.hpp"
#include <vector>

using namespace xr;

namespace
{

std::vector<char> buffer;
size_t replacedSize;

XM_TEST(StringUtils_Replace, Basic) // replace all instances in a string.
{
  auto original = "and and adn and or anda band";

  buffer.resize(strlen(original));
  auto result = Replace(original, "and", "ad", buffer.size(), buffer.data(), replacedSize);

  XM_ASSERT_EQ(result, buffer.data());
  XM_ASSERT_EQ(std::string(buffer.data(), replacedSize), "ad ad adn ad or ada bad");
}

XM_TEST(StringUtils_Replace, PartialMatches) // must not replace partial matches at beginning and end of non-null-terminated string.
{
  auto original = "and and adn and or anda band";

  buffer.resize(strlen(original));
  auto result = Replace(original + 1, 13, "and", "ad", buffer.size(), buffer.data(), replacedSize);

  XM_ASSERT_EQ(result, buffer.data());
  XM_ASSERT_EQ(std::string(buffer.data(), replacedSize), "nd ad adn an");
}

XM_TEST(StringUtils_Replace, NullCharacters) // null character doesn't trick Replace() into thinking the string is over - it will.
{
  auto original = "and\0and\0adn\0and\0or\0anda\0band";

  buffer.resize(28);  // can't use strlen() due to \0 characters.
  auto result = Replace(original, 28, "and", "ad", buffer.size(), buffer.data(), replacedSize);

  XM_ASSERT_EQ(result, buffer.data());
  XM_ASSERT_EQ(memcmp(buffer.data(), "ad\0ad\0adn\0ad\0or\0ada\0bad", replacedSize), 0);  // cannot use str(n)cmp() or std::string::operator==() due to \0 characters.

  // The overload that doesn't take size and determines it from strlen() shall
  // stop at the first \0 character.
  result = Replace(original, "and", "ad", buffer.size(), buffer.data(), replacedSize);
  XM_ASSERT_EQ(result, buffer.data());
  XM_ASSERT_EQ(std::string(buffer.data(), replacedSize), "ad");
}

XM_TEST(StringUtils_Replace, BufferTooSmall) // must stop at buffer boundary, even if it means that some of the original string doesn't fit.
{
  auto original = "and and adn and or anda band";

  buffer.resize(strlen(original));
  auto result = Replace(original, "and", "annd", buffer.size(), buffer.data(), replacedSize);

  XM_ASSERT_EQ(result, buffer.data());
  XM_ASSERT_EQ(std::string(buffer.data(), replacedSize), "annd annd adn annd or annda "); // "bannd" is pushed out.
}

XM_TEST(StringUtils_Replace, EmptyString) // delete all instances of 'find' string.
{
  auto original = "and and adn and or anda band";

  buffer.resize(strlen(original));
  auto result = Replace(original, "and", "", buffer.size(), buffer.data(), replacedSize);

  XM_ASSERT_EQ(result, buffer.data());
  XM_ASSERT_EQ(std::string(buffer.data(), replacedSize), "  adn  or a b");
}

XM_TEST(StringUtils_Replace, FindEmptyString) // empty string is matched everywhere, therefore the buffer is filled up with the 'replace' string.
{
  auto original = "and and adn and or anda band";

  buffer.resize(strlen(original));
  auto result = Replace(original, "", "and", buffer.size(), buffer.data(), replacedSize);

  XM_ASSERT_EQ(result, buffer.data());
  XM_ASSERT_EQ(std::string(buffer.data(), replacedSize), "andandandandandandandandanda");
}

XM_TEST(StringUtils_Replace, FindLongerThanOriginal) // find string is longer than original (but matching all the way) - copy original to buffer and don't do anything else.
{
  auto original = "and a";

  buffer.resize(strlen(original));
  auto result = Replace(original, "and and", "addn", buffer.size(), buffer.data(), replacedSize);

  XM_ASSERT_EQ(result, buffer.data());
  XM_ASSERT_EQ(std::string(buffer.data(), replacedSize), "and a");
}

XM_TEST(StringUtils, Char2Hex)
{
  char arBuffer[8];
  char* write = arBuffer;

  write = Char2Hex(0x0f, write);
  XM_ASSERT_EQ(write, arBuffer + 2);

  write = Char2Hex(char(0xf0), write);
  write = Char2Hex(char(0x77), write);
  write = Char2Hex(char(0x00), write);

  XM_ASSERT_EQ(std::string(arBuffer, sizeof(arBuffer)), "0ff07700");
}

}

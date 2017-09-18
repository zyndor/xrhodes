#include <gtest/gtest.h>
#include <XR/stringutils.hpp>
#include <vector>

namespace XR
{
    std::vector<char> buffer;
    size_t replacedSize;

    TEST(StringUtils_Replace, Basic) // replace all instances in a string.
    {
      auto original = "and and adn and or anda band";

      buffer.resize(strlen(original));
      auto result = Replace(original, "and", "ad", buffer.size(), buffer.data(), replacedSize);

      ASSERT_EQ(result, buffer.data());
      ASSERT_EQ(std::string(buffer.data(), replacedSize), "ad ad adn ad or ada bad");
    }

    TEST(StringUtils_Replace, PartialMatches) // must not replace partial matches at beginning and end of non-null-terminated string.
    {
      auto original = "and and adn and or anda band";

      buffer.resize(strlen(original));
      auto result = Replace(original + 1, 13, "and", "ad", buffer.size(), buffer.data(), replacedSize);

      ASSERT_EQ(result, buffer.data());
      ASSERT_EQ(std::string(buffer.data(), replacedSize), "nd ad adn an");
    }

    TEST(StringUtils_Replace, NullCharacters) // null character doesn't trick Replace() into thinking the string is over - it will.
    {
      auto original = "and\0and\0adn\0and\0or\0anda\0band";

      buffer.resize(28);  // can't use strlen() due to \0 characters.
      auto result = Replace(original, 28, "and", "ad", buffer.size(), buffer.data(), replacedSize);

      ASSERT_EQ(result, buffer.data());
      ASSERT_EQ(memcmp(buffer.data(), "ad\0ad\0adn\0ad\0or\0ada\0bad", replacedSize), 0);  // cannot use str(n)cmp() or std::string::operator==() due to \0 characters.

      // The overload that doesn't take size and determines it from strlen() shall
      // stop at the first \0 character.
      result = Replace(original, "and", "ad", buffer.size(), buffer.data(), replacedSize);
      ASSERT_EQ(result, buffer.data());
      ASSERT_EQ(std::string(buffer.data(), replacedSize), "ad");
    }

    TEST(StringUtils_Replace, BufferTooSmall) // must stop at buffer boundary, even if it means that some of the original string doesn't fit.
    {
      auto original = "and and adn and or anda band";

      buffer.resize(strlen(original));
      auto result = Replace(original, "and", "annd", buffer.size(), buffer.data(), replacedSize);

      ASSERT_EQ(result, buffer.data());
      ASSERT_EQ(std::string(buffer.data(), replacedSize), "annd annd adn annd or annda "); // "bannd" is pushed out.
    }

    TEST(StringUtils_Replace, EmptyString) // delete all instances of 'find' string.
    {
      auto original = "and and adn and or anda band";

      buffer.resize(strlen(original));
      auto result = Replace(original, "and", "", buffer.size(), buffer.data(), replacedSize);

      ASSERT_EQ(result, buffer.data());
      ASSERT_EQ(std::string(buffer.data(), replacedSize), "  adn  or a b");
    }

    TEST(StringUtils_Replace, FindEmptyString) // empty string is matched everywhere, therefore the buffer is filled up with the 'replace' string.
    {
      auto original = "and and adn and or anda band";

      buffer.resize(strlen(original));
      auto result = Replace(original, "", "and", buffer.size(), buffer.data(), replacedSize);

      ASSERT_EQ(result, buffer.data());
      ASSERT_EQ(std::string(buffer.data(), replacedSize), "andandandandandandandandanda");
    }

    TEST(StringUtils_Replace, FindLongerThanOriginal) // find string is longer than original (but matching all the way) - copy original to buffer and don't do anything else.
    {
      auto original = "and a";

      buffer.resize(strlen(original));
      auto result = Replace(original, "and and", "addn", buffer.size(), buffer.data(), replacedSize);

      ASSERT_EQ(result, buffer.data());
      ASSERT_EQ(std::string(buffer.data(), replacedSize), "and a");
    }

    TEST(StringUtils, Char2Hex)
    {
      char arBuffer[8];
      char* write = arBuffer;

      write = Char2Hex(0x0f, write);
      ASSERT_EQ(write, arBuffer + 2);

      write = Char2Hex(char(0xf0), write);
      write = Char2Hex(char(0x77), write);
      write = Char2Hex(char(0x00), write);

      ASSERT_EQ(std::string(arBuffer, sizeof(arBuffer)), "0ff07700");
    }
}

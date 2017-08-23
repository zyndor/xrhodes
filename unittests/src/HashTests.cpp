#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/StringFormatter.hpp>
#include <XR/Hash.hpp>
#include <XR/utils.hpp>
#include <map>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  char const* caseCmpString1 = "OnE - oF My 962946 CrAzY_CasE StRiNgs!?.";
  char const* caseCmpString2 = "oNe - Of mY 962946 cRaZy_cASe sTrInGS!?.";

  char const* strings[] =
  {
    "idle",
    "sneak",
    "walk",
    "run",
    "damaged",
    "crouch",
    "dance",
    "punch",
    "kick",
    "dodge",
    "parry",
    "throw",
    "taunt",
    "shoot",
    "flirt",
    "block",
    "die",
    "celebrate",
    "roll",
    "interact",
  };

  TEST_CLASS(HashTests)
  {
  public:
    TEST_METHOD(Hash_StringCaseInsensitive)
    {
      auto hash1 = Hash::String(caseCmpString1);
      auto hash2 = Hash::String(caseCmpString2);
      Assert::IsTrue(hash1 == hash2);
    }

    TEST_METHOD(Hash_String32CaseInsensitive)
    {
      auto hash1 = Hash::String32(caseCmpString1);
      auto hash2 = Hash::String32(caseCmpString2);
      Assert::IsTrue(hash1 == hash2);
    }

    template <typename T>
    std::wstring MakeAssertMsg(char const* s0, char const* s1, T hash)
    {
      std::wostringstream buf;
      buf << s0 << " vs " << s1 << " (" << std::ios::hex << hash << ") clash.";
      return buf.str();
    }

    void Format(char const* name, uint32_t hash, char(&buffer)[64])
    {
      sprintf(buffer, "%s:  %x", name, hash);
    }

    void Format(char const* name, uint64_t hash, char(&buffer)[64])
    {
      sprintf(buffer, "%s:  %llx", name, hash);
    }

    template <typename T>
    void DumpHashes(std::map<T, std::string> const& hashes)
    {
      std::map<unsigned char, int> occurences;

      char arBuffer[64];
      for (auto const& i : hashes)
      {
        ++occurences[(i.first >> (XR_BITSIZEOF(T) - 8)) & 0xff];
        Format(i.second.c_str(), i.first, arBuffer);
        Logger::WriteMessage(arBuffer);
      }

      int max = 0;
      unsigned char cMax = 0;
      int min = 9999;
      unsigned char cMin = 0;
      for (auto const& i : occurences)
      {
        if (i.second > max)
        {
          max = i.second;
          cMax = i.first;
        }
        if (i.second < min)
        {
          min = i.second;
          cMin = i.first;
        }
        sprintf(arBuffer, "%x : %d", i.first, i.second);
        Logger::WriteMessage(arBuffer);
      }

      sprintf(arBuffer, "min: %d (%x)", min, cMin);
      Logger::WriteMessage(arBuffer);

      sprintf(arBuffer, "max: %d (%x)", max, cMax);
      Logger::WriteMessage(arBuffer);
    }

    void DoStringUniqueness(bool dump)
    {
      std::map<uint64_t, std::string> hashes;
      char arBuffer[256];
      for (int i = 0; i < 26; ++i)
      {
        for (int j = 0; j < 10; ++j)
        {
          for (int z = 0; z < XR_ARRAY_SIZE(strings); ++z)
          {
            auto len = sprintf(arBuffer, "ins_%c_%s%d_anims", i + 'a', strings[z], j);
            auto hash = Hash::String(arBuffer, false);

            auto iFind = hashes.find(hash);
            bool assertion = iFind == hashes.end();
            Assert::IsTrue(assertion, assertion ? nullptr : MakeAssertMsg(arBuffer, iFind->second.c_str(), hash).c_str());
            hashes[hash] = arBuffer;
          }
        }
      }

      if(dump) DumpHashes(hashes);
    }

    TEST_METHOD(Hash_StringUniqueness)
    {
      Hash::SetSeed(Hash::kSeed);
      DoStringUniqueness(true);

      for (int i = 0; i < 32; ++i)
      {
        Hash::SetSeed((1 << i) - 1);
        DoStringUniqueness(false);
      }
    }

    void DoString32Uniqueness(bool dump)
    {
      std::map<uint32_t, std::string> hashes;
      char arBuffer[256];
      for (int i = 0; i < 26; ++i)
      {
        for (int j = 0; j < 10; ++j)
        {
          for (int z = 0; z < XR_ARRAY_SIZE(strings); ++z)
          {
            auto len = sprintf(arBuffer, "ins_%c_%s%d_anims", i + 'a', strings[z], j);
            auto hash = Hash::String32(arBuffer, len);

            auto iFind = hashes.find(hash);
            bool assertion = iFind == hashes.end();
            Assert::IsTrue(assertion, assertion ? nullptr : MakeAssertMsg(arBuffer, iFind->second.c_str(), hash).c_str());
            hashes[hash] = arBuffer;
          }
        }
      }

      if (dump) DumpHashes(hashes);
    }

    TEST_METHOD(Hash_String32Uniqueness)
    {
      Hash::SetSeed(Hash::kSeed);
      DoString32Uniqueness(true);

      for (int i = 0; i <= 32; ++i)
      {
        Hash::SetSeed((1 << i) - 1);
        DoString32Uniqueness(false);
      }
    }

  };
}

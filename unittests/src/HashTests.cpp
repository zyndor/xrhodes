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
#include "xr/utility/Hash.hpp"
#include "xr/utils.hpp"
#include <map>
#include <set>

using namespace xr;

namespace
{

class Hash
{
public:
  ~Hash()
  {
    xr::Hash::SetSeed(xr::Hash::kSeed);
  }
};

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

XM_TEST_F(Hash, StringCaseInsensitive)
{
  auto hash1 = xr::Hash::String(caseCmpString1);
  auto hash2 = xr::Hash::String(caseCmpString2);
  XM_ASSERT_EQ(hash1, hash2);
}

XM_TEST_F(Hash, String32CaseInsensitive)
{
  auto hash1 = xr::Hash::String32(caseCmpString1);
  auto hash2 = xr::Hash::String32(caseCmpString2);
  XM_ASSERT_EQ(hash1, hash2);
}

template <typename T>
std::wstring MakeAssertMsg(char const* s0, char const* s1, T hash)
{
  std::wostringstream buf;
  buf << s0 << " vs " << s1 << " (" << std::hex << hash << ") clash.";
  return buf.str();
}

void Format(char const* name, uint32_t hash, char(&buffer)[64])
{
  sprintf(buffer, "%s:\t%x", name, hash);
}

void Format(char const* name, uint64_t hash, char(&buffer)[64])
{
  sprintf(buffer, "%s:\t%llx", name, hash);
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
    XR_TRACE(Hash, (arBuffer));
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
    XR_TRACE(Hash, (arBuffer));
  }

  sprintf(arBuffer, "min: %d (%x)", min, cMin);
  XR_TRACE(Hash, (arBuffer));

  sprintf(arBuffer, "max: %d (%x)", max, cMax);
  XR_TRACE(Hash, (arBuffer));
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
        auto hash = xr::Hash::String(arBuffer, len, false);

        auto iFind = hashes.find(hash);
        bool hashClashing = iFind != hashes.end();
        XR_TRACEIF(Hash, hashClashing, ("%s", MakeAssertMsg(arBuffer, iFind->second.c_str(), hash).c_str()));
        XM_ASSERT_FALSE(hashClashing);
        hashes[hash] = arBuffer;
      }
    }
  }

  if(dump) DumpHashes(hashes);
}

XM_TEST_F(Hash, StringUniqueness)
{
  xr::Hash::SetSeed(xr::Hash::kSeed);
  DoStringUniqueness(false);

  for (int i = 0; i < 32; ++i)
  {
    xr::Hash::SetSeed((1 << i) - 1);
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
        auto hash = xr::Hash::String32(arBuffer, len);

        auto iFind = hashes.find(hash);
        bool hashClashing = iFind != hashes.end();
        XR_TRACEIF(Hash, hashClashing, ("%s", MakeAssertMsg(arBuffer, iFind->second.c_str(), hash).c_str()));
        XM_ASSERT_FALSE(hashClashing);
        hashes[hash] = arBuffer;
      }
    }
  }

  if (dump) DumpHashes(hashes);
}

XM_TEST_F(Hash, String32Uniqueness)
{
  xr::Hash::SetSeed(xr::Hash::kSeed);
  DoString32Uniqueness(false);

  for (int i = 0; i <= 32; ++i)
  {
    xr::Hash::SetSeed((1 << i) - 1);
    DoString32Uniqueness(false);
  }
}

//XM_TEST_F(Hash, ShortString32Uniqueness)
//{
//  char const kAlphabet[] = "abcdefghijklmnopqrstuvwxyz";// 0123456789";
//  const auto kAlphaSize = XR_ARRAY_SIZE(kAlphabet) - 1;
//  char buffer[6];
//  char logBuffer[64];
//  std::map<uint32_t, std::string> hashes;
//  //std::set<uint32_t> hashes;
//  for (int k = 2; k < kAlphaSize; ++k)
//  {
//    int target = pow(kAlphaSize, k);
//    for (int j = 0; j < target; ++j)
//    {
//      int jj = j;
//      for (int i = k - 1; i >= 0; --i)
//      {
//        buffer[i] = kAlphabet[jj % kAlphaSize];
//        jj /= kAlphaSize;
//      }

//      auto hash = Hash::String32(buffer, k);
//      auto iFind = hashes.find(hash);
//      bool assertion = iFind == hashes.end();
//      XM_ASSERT_TRUE(assertion, assertion ? nullptr : MakeAssertMsg(buffer, iFind->second.c_str(), hash).c_str());
//      hashes.insert(iFind, { hash, std::string(buffer, k) });
//      //XM_ASSERT_TRUE(assertion, assertion ? nullptr : MakeAssertMsg(buffer, buffer, hash).c_str());
//      //hashes.insert(hash);
//      //
//      //Format(buffer, hash, logBuffer);
//      //XR_TRACE(Hash, (logBuffer));
//    }
//  }
//}

}

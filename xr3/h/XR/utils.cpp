//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <cstring>
#include "utils.hpp"
#include "Hash.hpp"
#include "debug.hpp"

namespace XR
{

//==============================================================================
std::string Char2Hex(char c)
{
  char dig1((c & 0xf0) >> 4);
  char dig2(c & 0x0f);
  if (dig1 >= 0x0 && dig1 <= 0x9) dig1 += '0'; //0,48 in ascii
  if (dig1 >= 0xa && dig1 <= 0xf) dig1 += 'A' - 0xa; //a,97 in ascii
  if (dig2 >= 0x0 && dig2 <= 0x9) dig2 += '0';
  if (dig2 >= 0xa && dig2 <= 0xf) dig2 += 'A' - 0xa;
  std::string r;
  r.append(&dig1, 1);
  r.append(&dig2, 1);
  return r;
}

//==============================================================================
std::string UrlEncode(const char* pString)
{
  XR_ASSERT(UrlEncode, pString != 0);
  std::ostringstream escaped;
  int max = strlen(pString);
  for (const char* p1(pString + max); pString != p1; ++pString)
  {
    char  c(*pString);
    if ((c >= '0' && c <= '9') ||  // 0-9
      (c >= 'A' && c <= 'Z') || // ABC...XYZ
      (c >= 'a' && c <= 'z') || // abc...xyz
      (c =='-' || c =='_' || c =='.' || c=='~'))
    {
      escaped << c;
    }
    else
    {
      escaped << '%';
      escaped << Char2Hex(c);
    }
  }
  return escaped.str();
}

} // XR

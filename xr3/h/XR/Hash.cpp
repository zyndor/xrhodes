//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Hash.hpp"
#include "debug.hpp"
#include <algorithm>
#include <map>
#include <string>
#include <ctype.h>
#include <string.h>

namespace XR
{

//==============================================================================
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
namespace
{
typedef std::map<uint32_t, std::string> StringMap;

StringMap  s_stringLookup;

void SquashCase(std::string& str)
{
  std::transform(str.begin(), str.end(), str.begin(), [](std::string::value_type c)
    { return tolower(c); });
}

}
#endif  //XR_DEBUG

//==============================================================================
uint32_t  Hash::s_seed(Hash::kSeed);

//==============================================================================
void  Hash::SetSeed(uint32_t seed)
{
  s_seed = seed;
}

//==============================================================================
uint32_t  Hash::String(const char* pString)
{
  XR_ASSERT(Hash, pString != 0);
  uint32_t hash = s_seed;
  int c;
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  std::string squashed(pString);
  SquashCase(squashed);
#endif  //XR_DEBUG
  while ((c = *pString) != 0)
  {
    hash += (hash << 5) + (hash + tolower(c));
    ++pString;
  }

#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  StringMap::iterator iFind(s_stringLookup.find(hash));
  const bool notFound = iFind == s_stringLookup.end();
  XR_ASSERTMSG(Hash, notFound || squashed == iFind->second,
    ("Hash collision - %s vs %s (%d)", squashed.c_str(), iFind->second.c_str(), hash));
  if (notFound)
  {
    s_stringLookup[hash] = squashed;
  }
#endif  //XR_DEBUG

  return hash;
}

//==============================================================================
uint32_t  Hash::String(const char* pString, size_t size)
{
  XR_ASSERT(Hash, pString != 0);
  uint32_t hash = s_seed;
  int c;
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  std::string squashed(pString, size);
  SquashCase(squashed);
#endif  //XR_DEBUG
  while (size > 0 && (c = *pString) != 0)
  {
    hash += (hash << 5) + (hash + tolower(c));
    ++pString;
    --size;
  }

#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  StringMap::iterator iFind(s_stringLookup.find(hash));
  // look for index of first non matching character being @a size
  const bool notFound = iFind == s_stringLookup.end();
  XR_ASSERTMSG(Hash, notFound || squashed == iFind->second,
    ("Hash collision - %s vs %s (%d)", squashed.c_str(), iFind->second.c_str(), hash));
  if (notFound)
  {
    s_stringLookup[hash] = squashed;
  }
#endif  //XR_DEBUG

  return hash;
}

//==============================================================================
uint32_t  Hash::Data(const void* pData, size_t size)
{
  XR_ASSERT(Hash, size >= 0);
  uint32_t hash = s_seed;
  const char* p0(static_cast<const char*>(pData));
  const char* p1(p0 + size);
  while (p0 != p1)
  {
    hash += (hash << 5) + (hash + *p0);
    ++p0;
  }
  return hash;
}

//==============================================================================
void  Hash::DebugClearStringLookup()
{
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  s_stringLookup.clear();
#endif
}

} // XR
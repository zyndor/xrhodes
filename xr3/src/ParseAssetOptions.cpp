//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "ParseAssetOptions.hpp"
#include <regex>

namespace xr
{
#ifdef ENABLE_ASSET_BUILDING
namespace
{
// NOTE: it'd be nice to have look-behind ((?<=$)) - us in C++ don't.
const std::regex kDefineRegex("(\\$[^$\\.\\s]+)");
}
#endif

bool ParseAssetOptions(const char* str, std::function<bool(char const*)> onOption)
{
#ifdef ENABLE_ASSET_BUILDING
  std::cregex_iterator iDefines(str, str + strlen(str), kDefineRegex);
  std::cregex_iterator iDefinesEnd;
  while (iDefines != iDefinesEnd)
  {
    auto result = *iDefines;
    if (result.size() > 1)
    {
      auto sResult = result.str().substr(1);
      if (!onOption(sResult.c_str()))
      {
        return false;
      }
    }
    ++iDefines;
  }
  return true;
#else
  return false;
#endif
}

} // xr

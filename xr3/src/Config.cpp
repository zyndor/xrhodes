//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Config.hpp"
#include "xr/strings/stringutils.hpp"
#include "xr/debug.hpp"
#include <cstdlib>

namespace xr
{

//==============================================================================
std::string Config::Get(char const* varName)
{
  XR_ASSERT(Config, varName != nullptr);
  return GetStringSafe(std::getenv(varName));
}

//==============================================================================
int32_t Config::GetInt(char const* varName, int32_t defaultValue)
{
  XR_ASSERT(Config, varName != nullptr);
  auto value = GetStringSafe(std::getenv(varName));
  if (value[0] && !StringTo(value, defaultValue))
  {
    XR_TRACE(Config, ("Failed to convert the value '%s' of '%s' to integer; using %d",
      value, varName, defaultValue));
  }
  return defaultValue;
}

}

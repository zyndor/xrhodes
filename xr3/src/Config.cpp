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
int Config::GetInt(char const* varName, int defaultValue)
{
  std::string  value(Get(varName));
  if (!value.empty())
  {
    if (!StringTo(value.c_str(), defaultValue))
    {
      XR_TRACE(Config, ("Failed to convert the value '%s' of '%s' to integer; using %d",
        value.c_str(), varName, defaultValue));
    }
  }
  return defaultValue;
}

}

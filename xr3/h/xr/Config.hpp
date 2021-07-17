#ifndef XR_CONFIG_HPP
#define XR_CONFIG_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/types/fundamentals.hpp"
#include <string>

namespace xr
{

//==============================================================================
///@brief Facilitates access to configuration options stored in environment
/// variables. Those used by XRhodes are typically prefixed with XR_.
class Config
{
  XR_NONOBJECT_DECL(Config)

public:
  ///@brief Gets a configuration option stored under the given @a varName.
  ///@return The string value of the configuration option; empty string if
  /// undefined.
  static std::string Get(char const* varName);

  ///@brief Attempts to get a configuration option, stored under @a varName,
  /// as an integer.
  ///@return The integer value of the configuration option; @a defaultValue
  /// if undefined or not castable to an integer.
  static int32_t GetInt(char const* varName, int32_t defaultValue);
};

}

#endif //XR_CONFIG_HPP

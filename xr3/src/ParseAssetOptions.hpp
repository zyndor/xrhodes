#ifndef XR_PARSE_ASSET_OPTIONS_HPP
#define XR_PARSE_ASSET_OPTIONS_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <functional>

namespace xr
{

///@brief Locates asset options, starting with a $ sign, followed by at least one
/// character that is not a space, period (.) or $, in @a str, and passes them to
/// @a onOption. @a onOption may return false to stop processing.
bool ParseAssetOptions(const char* str, std::function<bool(char const*)> onOption);

} // xr

#endif  //XR_PARSE_ASSET_OPTIONS_HPP

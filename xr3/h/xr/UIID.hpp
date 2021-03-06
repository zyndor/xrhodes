#ifndef XR_UIID_HPP
#define XR_UIID_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include <vector>
#include <cstdint>

namespace xr
{

typedef std::vector<uint8_t>  UIID;

extern const size_t kUIIDSize;

//==============================================================================
// Generates a 16-byte Unique Instance Identifier.
//==============================================================================
UIID  GenerateUIID();

} // xr

#endif  //XR_UIID_HPP

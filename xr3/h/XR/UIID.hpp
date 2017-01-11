//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UIID_HPP
#define XR_UIID_HPP

#include <vector>
#include <cstdint>

namespace XR
{

typedef std::vector<uint8_t>  UIID;

extern const size_t kUIIDSize;

//==============================================================================
// Generates a 16-byte Unique Instance Identifier.
//==============================================================================
UIID  GenerateUIID();

} // XR

#endif  //XR_UIID_HPP
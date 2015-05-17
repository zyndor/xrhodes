//
// Nuclear Heart Games
// XRhodes
//
// UIID.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    16/05/2015
//
// copyright (c) 2011 - 2015. All rights reserved.
//
//==============================================================================
#if !defined XR_UIID_HPP
#define XR_UIID_HPP

#include  <vector>
#include  "types.hpp"

namespace XR
{

typedef std::vector<uint8>  UIID;

//==============================================================================
// Generates a 16-byte Unique Instance Identifier.
//==============================================================================
UIID  GenerateUIID();

} // XR

#endif  //XR_UIID_HPP
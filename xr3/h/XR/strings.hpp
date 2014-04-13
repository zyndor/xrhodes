//
// Nuclear Heart Games
// XRhodes
//
// strings.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    16/01/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_STRINGS_HPP
#define XR_STRINGS_HPP

#include "HardString.hpp"

namespace XR
{

//==============================================================================
typedef HardString<16>   TString;
typedef HardString<64>   SString;
typedef HardString<256>  MString;
typedef HardString<1024> LString;

} // XR

#endif // XR_STRINGS_HPP

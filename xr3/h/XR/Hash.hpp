//
// Nuclear Heart Games
// XRhodes
//
// Hash.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    10/08/2013
//
// copyright (c) 2011 - 2015. All rights reserved.
//
//==============================================================================
#if !defined XR_HASH_HPP
#define XR_HASH_HPP

#include "types.hpp"

namespace XR
{

//==============================================================================
class Hash
{
  XR_NONOBJECT_DECL(Hash)

public:
  // static
  static const uint32 kSeed = 61681;

  static uint32 String(const char* pString, uint32 hash = kSeed);
  static uint32 String(const char* pString, int32 size, uint32 hash = kSeed);
  static uint32 Data(const void* pData, int32 size, uint32 hash = kSeed);

  static void   DebugClearStringLookup();
};

} // XR

#endif // XR_HASH_HPP
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_HASH_HPP
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
  static const uint32_t kSeed = 61681;

  static void     SetSeed(uint32_t seed);
  
  static uint32_t String(const char* pString);
  static uint32_t String(const char* pString, size_t size);
  static uint32_t Data(const void* pData, size_t size);

  static void     DebugClearStringLookup();

private:
  static uint32_t s_seed;
};

} // XR

#endif // XR_HASH_HPP
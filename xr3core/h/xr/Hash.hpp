#ifndef XR_HASH_HPP
#define XR_HASH_HPP
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
#include <cstdint>

namespace xr
{

//==============================================================================
///@brief Non-cryptographic hashing of binary data and strings (case
/// insensitive). 32-bit version is primitive add-rotate-xor based
/// implementation; 64-bit version is based on Austin Appleby's MurmurHash2.
class Hash
{
  XR_NONOBJECT_DECL(Hash)

public:
  // static
  static const uint64_t kSeed = 61681;  // cast to uint32_t when used in 32-bit variants.

  static void     SetSeed(uint64_t seed);

  static uint32_t String32(const char* data);
  static uint32_t String32(const char* data, size_t size);
  static uint32_t Data32(const void* data, size_t size);

  static uint64_t String(const char* string);
  static uint64_t String(const char* string, size_t size);
  static uint64_t Data(const void* data, size_t size);

  [[deprecated("Drop the last argument")]]
  static uint64_t String(const char* string, bool assertUnique);

  [[deprecated("Drop the last argument")]]
  static uint64_t String(const char* string, size_t size, bool assertUnique);
};

} // xr

#endif // XR_HASH_HPP

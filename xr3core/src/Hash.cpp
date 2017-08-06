//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/Hash.hpp>
#include <XR/debug.hpp>
#include <algorithm>
#include <map>
#include <string>
#include <ctype.h>
#include <string.h>

namespace XR
{
namespace
{

uint8_t ByteNoOp(uint8_t c)
{
  return c;
}

uint8_t ToLower(uint8_t c)
{
  return (c >= 'A' && c <= 'Z') ? c + 'a' - 'A' : c;
}

//==============================================================================
template <uint8_t(*byteOp)(uint8_t)>
uint32_t Hash32(const void* data, size_t size, uint32_t seed)
{
  uint32_t hash = seed;
  auto p = static_cast<uint8_t const*>(data);
  auto endp = p + size;
  uint32_t prev;
  while (p != endp)
  {
    prev = hash >> 31;
    hash += (hash << 5) + (hash + byteOp(*p));
    hash ^= prev;
    ++p;
  }
  return hash;
}

#define PREPROCESS_BYTES(x, byteOp)\
  (uint32_t(byteOp((x) >> 24)) << 24) |\
  (uint32_t(byteOp(((x) >> 16) & 0xff)) << 16) |\
  (uint32_t(byteOp(((x) >> 8) & 0xff)) << 8) |\
  (uint32_t(byteOp((x) & 0xff)))

//==============================================================================
// Based on MurmurHash2, written and placed in public domain by Austin Appleby.
// The author hereby disclaims copyright to this source code.
template <uint8_t (*byteOp)(uint8_t)>
uint64_t MurmurHash64B(void const* key, size_t len, uint64_t seed)
{
  const uint32_t m = 0x5bd1e995;
  const int r = 24;

  uint32_t h1 = uint32_t(seed) ^ len;
  uint32_t h2 = uint32_t(seed >> 32);

  const uint32_t * data = (const uint32_t *)key;

  while(len >= 8)
  {
    uint32_t k1 = *data++;
    k1 = PREPROCESS_BYTES(k1, byteOp);
    k1 *= m; k1 ^= k1 >> r; k1 *= m;
    h1 *= m; h1 ^= k1;
    len -= 4;

    uint32_t k2 = *data++;
    k2 = PREPROCESS_BYTES(k2, byteOp);
    k2 *= m; k2 ^= k2 >> r; k2 *= m;
    h2 *= m; h2 ^= k2;
    len -= 4;
  }

  if(len >= 4)
  {
    uint32_t k1 = *data++;
    k1 = PREPROCESS_BYTES(k1, byteOp);
    k1 *= m; k1 ^= k1 >> r; k1 *= m;
    h1 *= m; h1 ^= k1;
    len -= 4;
  }

  switch(len)
  {
  case 3: h2 ^= uint32_t(byteOp(((unsigned char*)data)[2])) << 16;
  case 2: h2 ^= uint32_t(byteOp(((unsigned char*)data)[1])) << 8;
  case 1: h2 ^= uint32_t(byteOp(((unsigned char*)data)[0]));
      h2 *= m;
  };

  h1 ^= h2 >> 18; h1 *= m;
  h2 ^= h1 >> 22; h2 *= m;
  h1 ^= h2 >> 17; h1 *= m;
  h2 ^= h1 >> 19; h2 *= m;

  uint64_t h = h1;

  h = (h << 32) | h2;

  return h;
} 

#undef PREPROCESS_BYTES
}

//==============================================================================
uint64_t  s_seed(Hash::kSeed);

//==============================================================================
void  Hash::SetSeed(uint64_t seed)
{
  s_seed = seed;
}

//==============================================================================
uint32_t Hash::String32(const char* str)
{
  return String32(str, strlen(str));
}

//==============================================================================
uint32_t Hash::String32(const char* str, size_t size)
{
  return Hash32<ToLower>(str, size, s_seed);
}

//==============================================================================
uint32_t Hash::Data32(const void* data, size_t size)
{
  return Hash32<ByteNoOp>(data, size, s_seed);
}

//==============================================================================
uint64_t  Hash::String(const char* pString, bool assertUnique)
{
  return String(pString, strlen(pString), assertUnique);
}

//==============================================================================
uint64_t  Hash::String(const char* pString, size_t size, bool assertUnique)
{
  return MurmurHash64B<ToLower>(pString, size, s_seed);
}

//==============================================================================
uint64_t  Hash::Data(const void* pData, size_t size)
{
  return MurmurHash64B<ByteNoOp>(pData, size, s_seed);
}

} // XR

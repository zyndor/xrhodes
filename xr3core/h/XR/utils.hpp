//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UTILS_HPP
#define XR_UTILS_HPP

#include "XR/debug.hpp"
#include <sstream>
#include <algorithm>
#include <cstdint>

//==============================================================================
namespace
{
template <typename T, size_t N>
char(&ArraySizeHelper(T (&)[N]))[N];  // not defined, never actually called; sizeof(function call) tells size of return type.
}
#define XR_ARRAY_SIZE(a) (sizeof(ArraySizeHelper(a)))

//==============================================================================
///@brief Converts @a id into a mask with the @a id'th bit set.
#define XR_MASK_ID(type, id) static_cast<type>(1 << (id))

//==============================================================================
#define XR_BITSIZEOF(x) (sizeof(x) * 8)

//==============================================================================
///@brief Converts four characters' worth of human readable information into
/// a uint32_t.
#define XR_FOURCC(a, b, c, d) ((static_cast<uint32_t>(static_cast<uint8_t>(d)) << 24) |\
  (static_cast<uint32_t>(static_cast<uint8_t>(static_cast<uint32_t>(c)) << 16)) |\
  (static_cast<uint32_t>(static_cast<uint8_t>(static_cast<uint32_t>(b)) << 8)) |\
  (static_cast<uint32_t>(static_cast<uint8_t>(static_cast<uint32_t>(a)))))

namespace XR
{

//==============================================================================
///@return Whether any bits of @a mask are set in @a bits.
bool CheckAnyMaskBits(uint32_t bits, uint32_t mask);

///@return Whether all bits of @a mask are set in @a bits.
bool CheckAllMaskBits(uint32_t bits, uint32_t mask);

///@return Whether the @a id-th bit matches @a mask?
bool CheckIdthBit(uint32_t bits, uint32_t id);

///@return Whether @a value can be fit into 16 bits.
bool  CheckInt16(int value);

///@return Index of item in array of values, @a numValues if not found.
template <typename T>
size_t FindItemId(T const* parValues, size_t numValues, T const& pValue);

///@brief Count number of elements in a null terminated array.
template <typename T>
size_t CountArrayItems(T const* pItems);

//==============================================================================
///@brief For find_if ()ing an object based on the pointer to it.
template  <typename T>
struct  PtrFindPredicate
{
  explicit PtrFindPredicate(const T* p)
  : pObject(p)
  {}

  bool  operator()(const T& a) const
  {
    return &a == pObject;
  }

  const T* const pObject;
};

//==============================================================================
///@brief For using pointers as keys in maps.
struct  PtrHash
{
  size_t  operator()(void const* p) const
  {
    return reinterpret_cast<size_t>(reinterpret_cast<char const*>(p));
  }
};

//==============================================================================
// implementation
//==============================================================================
inline
bool CheckAnyMaskBits(uint32_t bits, uint32_t mask)
{
  return (bits & mask) != 0;
}

//==============================================================================
inline
bool CheckAllMaskBits(uint32_t bits, uint32_t mask)
{
  return (bits & mask) == mask;
}

//==============================================================================
inline
bool CheckIdthBit(uint32_t bits, uint32_t id)
{
  return CheckAllMaskBits(bits, XR_MASK_ID(uint32_t, id));
}

//==============================================================================
inline
bool  CheckInt16(int value)
{
  return !(((value & 0xffff8000) + 0x8000) & 0xffff7fff);
}

//==============================================================================
template <typename T>
inline
size_t FindItemId(T const* parValues, size_t numValues, T const& item)
{
  XR_ASSERT(FindItemId, parValues != 0);
  XR_ASSERT(FindItemId, numValues >= 0);
  const T* pFind(std::find(parValues, parValues + numValues, item));
  return pFind - parValues;
}

//==============================================================================
template <typename T>
size_t CountArrayItems(T const* pItems)
{
  XR_ASSERT(CountArrayItems, pItems != nullptr);
  T const* pCursor = pItems;
  while (*pCursor != nullptr)
  {
    ++pCursor;
  }

  size_t count(pCursor - pItems);
  return count;
}

} // XR

#endif  //XR_UTILS_HPP

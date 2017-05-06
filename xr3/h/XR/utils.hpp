//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UTILS_HPP
#define XR_UTILS_HPP

#include <XR/debug.hpp>
#include <sstream>
#include <algorithm>
#include <cstdint>

//==============================================================================
namespace
{
template <typename T, size_t N>
char(&ArraySizeHelper(T (&)[N]))[N];
}
#define XR_ARRAY_SIZE(a) (sizeof(ArraySizeHelper(a)))

//==============================================================================
#define XR_MASK_ID(id) (1 << static_cast<size_t>(id))

namespace XR
{

//==============================================================================
// primitives
bool IsAllBits(uint32_t bits, uint32_t mask);
bool IsFullMask(uint32_t bits, uint32_t mask);
bool IsIdMask(uint32_t id, uint32_t mask);

///@brief Checks is an integer can be fit into 16bits.
bool  CheckInt16(int value);

///@brief
const char* GetStringSafe(const char* pString);

///@return Index of item in array of values, @a numValues if not found.
template <typename T>
size_t FindItemId(const T* parValues, size_t numValues, const T* pValue);

///@brief Count number of elements in a null terminated array.
template <typename T>
size_t CountArrayItems(T* pItems);

// strings
std::string UrlEncode(const char* pString);
std::string Char2Hex(char c);

template  <typename T>
bool  StringTo(const char* pString, T& to);

// resources
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
  size_t  operator()(const void* p) const
  {
    return (size_t)((const char*)p);
  }
};

//==============================================================================
// implementation
//==============================================================================
inline
bool IsAllBits(uint32_t bits, uint32_t mask)
{
  return (bits & mask) == bits;
}

//==============================================================================
inline
bool IsFullMask(uint32_t bits, uint32_t mask)
{
  return (bits & mask) == mask;
}

//==============================================================================
inline
bool IsIdMask(uint32_t id, uint32_t mask) 
{
  return IsFullMask(XR_MASK_ID(id), mask);
}

//==============================================================================
inline
bool  CheckInt16(int value)
{
  return !(((value & 0xffff8000) + 0x8000) & 0xffff7fff);
}

//==============================================================================
inline
const char* GetStringSafe(const char* pString)
{
  return pString != 0 ? pString : "";
}

//==============================================================================
template <typename T>
inline
size_t FindItemId(const T* parValues, int numValues, const T& item)
{
  XR_ASSERT(FindItemId, parValues != 0);
  XR_ASSERT(FindItemId, numValues >= 0);
  const T* pFind(std::find(parValues, parValues + numValues, item));
  return pFind - parValues;
}

//==============================================================================
template <typename T>
size_t CountArrayItems(T* pItems)
{
  XR_ASSERT(CountArrayItems, pItems != nullptr);

  size_t count(0);
  while (*pItems != nullptr)
  {
    ++pItems;
    ++count;
  }

  return count;
}

//==============================================================================
template  <typename T>
bool  StringTo(const char* pString, T& to)
{
  std::istringstream  iss(pString);

  return !(iss >> std::ws >> to).fail() && (iss >> std::ws).eof();
}

} // XR

#endif  //XR_UTILS_HPP
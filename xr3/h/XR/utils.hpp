//
// Nuclear Heart Games
// XRhodes
//
// utils.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    03/07/2012
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_UTILS_HPP
#define XR_UTILS_HPP

#include <sstream>
#include <algorithm>
#include "types.hpp"

namespace XR
{

//==============================================================================
// primitives
bool IsAllBits(uint32 bits, uint32 mask);
bool IsFullMask(uint32 bits, uint32 mask);
bool IsIdMask(uint32 id, uint32 mask);

///@brief Checks is an integer can be fit into 16bits.
bool  CheckInt16(int value);

///@brief
const char* GetStringSafe(const char* pString);

///@return Index of item in array of values, @a numValues if not found.
template <typename T>
int FindItemId(const T* parValues, int numValues, const T* pValue);

///@brief Count number of elements in a null terminated array.
template <typename T>
int CountArrayItems(T* pItems);

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
bool IsAllBits(uint32 bits, uint32 mask)
{
  return (bits & mask) == bits;
}

//==============================================================================
inline
bool IsFullMask(uint32 bits, uint32 mask)
{
  return (bits & mask) == mask;
}

//==============================================================================
inline
bool IsIdMask(uint32 id, uint32 mask) 
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
int FindItemId(const T* parValues, int numValues, const T& item)
{
  XR_ASSERT(GetIdFromList, parValues != 0);
  XR_ASSERT(GetIdFromList, numValues >= 0);
  const T* pFind(std::find(parValues, parValues + numValues, item));
  return pFind - parValues;
}

//==============================================================================
template <typename T>
int CountArrayItems(T* pItems)
{
  XR_ASSERT(CountArrayItems, pItems != 0);

  int count(0);
  while (*pItems != 0)
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
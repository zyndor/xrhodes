//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_HARDSTRING_HPP
#define XR_HARDSTRING_HPP

#include <string.h>
#include <ctype.h>
#include <algorithm>
#include "debug.hpp"

#ifdef _MSC_VER
// Suppress warnings for known safe uses of strcpy(), strncpy(), sprintf() etc.
#pragma warning(push)
#pragma warning(disable:4996)
#endif

namespace XR
{

//==============================================================================
///@brief Fixed capacity string class that doesn't allocate memory through its
/// lifespan.
template  <size_t N>
class HardString
{
public:
  // types
  enum
  {
    kCapacity = N - 1
  };
  
  // structors
  HardString();
  HardString(const char* pString);
  HardString(const char* pString, size_t size);
  explicit HardString(int32_t n);
  
  // general
  size_t          size() const;
  size_t          capacity() const;
  const char*     c_str() const;
  char*           data();

  const char*     find(const char* pSub) const;
  char*           find(const char* pSub);
  const char*     find(int32_t c) const;
  char*           find(int32_t c);

  const char*     rfind(const char* pSub) const;
  char*           rfind(const char* pSub);
  const char*     rfind(int32_t c) const;
  char*           rfind(int32_t c);

  HardString<N>   substr(size_t offs, size_t size) const;

  HardString<N>&  tolower();
  HardString<N>&  toupper();

  void            clear();
  HardString<N>&  assign(const char* p, size_t len);

  // operators
  HardString<N>&  operator =(const char* pString);
  HardString<N>&  operator =(const HardString<N>& str);
  HardString<N>&  operator =(int32_t n);

  HardString<N>&  operator +=(const char* pString);
  HardString<N>&  operator +=(const HardString<N>& str);
  HardString<N>&  operator +=(int32_t n);

  HardString<N>   operator +(const char* pString) const;
  HardString<N>   operator +(const HardString<N>& str) const;
  HardString<N>   operator +(int32_t n) const;

  const char&     operator[](size_t i) const;
  char&           operator[](size_t i);
  
  bool            operator==(const char* pString) const;
  bool            operator==(const HardString<N>& str) const;

  bool            operator<(const char* pString) const;
  bool            operator<(const HardString<N>& str) const;

protected:
  // data
  char  m_arBuffer[N];
};

template  <>
class HardString<0>
{};

template  <size_t N>
bool  operator!=(const HardString<N>& s, const char* pStr);

template  <size_t N>
bool  operator!=(const HardString<N>& s0, const HardString<N>& s1);

template  <size_t N>
bool  operator==(const char* pStr, const HardString<N>& s);

template  <size_t N>
bool  operator!=(const char* pStr, const HardString<N>& s);

template  <size_t N>
bool  operator<(const char* pStr, const HardString<N>& s);

//==============================================================================
// implementation
//==============================================================================
inline
char* strrstr(const char* haystack, const char* needle)
{
  size_t needleSize = strlen(needle);
  char const* p = haystack + strlen(haystack) - needleSize;
  while(p >= haystack &&
    strncmp(p, needle, needleSize) != 0)
  {
    --p;
  }
  return p < haystack ? 0 : const_cast<char*>(p);
}

//==============================================================================
template  <size_t N>
inline
HardString<N>::HardString()
{
  m_arBuffer[0] = '\0';
}
  
//==============================================================================
template  <size_t N>
inline
HardString<N>::HardString(const char* pString)
: HardString<N>(pString, strlen(pString))
{}

//==============================================================================
template  <size_t N>
inline
HardString<N>::HardString(const char* pString, size_t size)
{
  XR_ASSERT(HardString<>, pString != nullptr);
  XR_ASSERT(HardString<>, size <= capacity());
  strncpy(m_arBuffer, pString, size);
  m_arBuffer[size] = '\0';
}

//==============================================================================
template  <size_t N>
inline
HardString<N>::HardString(int32_t n)
{
  m_arBuffer[0] = '\0';
  operator+=(n);
}
  
//==============================================================================
template  <size_t N>
inline
size_t  HardString<N>::size() const
{
  return strlen(m_arBuffer);
}
  
//==============================================================================
template  <size_t N>
inline
size_t  HardString<N>::capacity() const
{
  return kCapacity;
}

//==============================================================================
template  <size_t N>
inline
const char* HardString<N>::c_str() const
{
  return m_arBuffer;
}

//==============================================================================
template  <size_t N>
inline
char* HardString<N>::data()
{
  return m_arBuffer;
}

//==============================================================================
template  <size_t N>
inline
const char* HardString<N>::find(const char* pSub) const
{
  return strstr(m_arBuffer, pSub);
}

//==============================================================================
template  <size_t N>
inline
char* HardString<N>::find(const char* pSub)
{
  return strstr(m_arBuffer, pSub);
}

//==============================================================================
template  <size_t N>
inline
const char* HardString<N>::find(int32_t c) const
{
  return strchr(m_arBuffer, c);
}

//==============================================================================
template  <size_t N>
inline
char* HardString<N>::find(int32_t c)
{
  return strchr(m_arBuffer, c);
}

//==============================================================================
template  <size_t N>
inline
const char* HardString<N>::rfind(const char* pSub) const
{
  return strrstr(m_arBuffer, pSub);
}

//==============================================================================
template  <size_t N>
inline
char* HardString<N>::rfind(const char* pSub)
{
  return strrstr(m_arBuffer, pSub);
}

//==============================================================================
template  <size_t N>
inline
const char* HardString<N>::rfind(int32_t c) const
{
  return strrchr(m_arBuffer, c);
}

//==============================================================================
template  <size_t N>
inline
char* HardString<N>::rfind(int32_t c)
{
  return strrchr(m_arBuffer, c);
}

//==============================================================================
template  <size_t N>
inline
HardString<N> HardString<N>::substr(size_t offs, size_t size) const
{
  XR_ASSERT(HardString<>, offs + size <= capacity());
  return HardString<N>(m_arBuffer + offs, size);
}

//==============================================================================
template  <size_t N>
inline
HardString<N>&  HardString<N>::tolower()
{
  std::transform(m_arBuffer, m_arBuffer + size(), m_arBuffer, ::tolower);
  return *this;
}

//==============================================================================
template  <size_t N>
inline
HardString<N>&  HardString<N>::toupper()
{
  std::transform(m_arBuffer, m_arBuffer + size(), m_arBuffer, ::toupper);
  return *this;
}

//==============================================================================
template  <size_t N>
inline
void  HardString<N>::clear()
{
  m_arBuffer[0] = '\0';
}

//==============================================================================
template  <size_t N>
HardString<N>&  HardString<N>::assign(const char* pStr, size_t size)
{
  XR_ASSERT(HardString<N>, pStr != 0);
  XR_ASSERT(HardString<N>, size <= capacity());
  strncpy(m_arBuffer, pStr, size);
  m_arBuffer[size] = '\0';
  return *this;
}

//==============================================================================
template  <size_t N>
inline
HardString<N>& HardString<N>::operator =(const char* pString)
{
  XR_ASSERT(HardString<N>, strlen(pString) <= kCapacity);
  strcpy(m_arBuffer, pString);
  return *this;
}

//==============================================================================
template  <size_t N>
inline
HardString<N>& HardString<N>::operator =(const HardString<N>& str)
{
  return (*this = str.c_str());
}

//==============================================================================
template  <size_t N>
HardString<N>& HardString<N>::operator =(int32_t n)
{
  char  arBuffer[16];
  sprintf(arBuffer, "%d", n);
  XR_ASSERT(HardString<>, strlen(arBuffer) < sizeof(arBuffer));
  return (*this = arBuffer);
}

//==============================================================================
template  <size_t N>
inline
HardString<N>& HardString<N>::operator +=(const char* pString)
{
  const size_t  len = size();
  XR_ASSERT(HardString<>, len + strlen(pString) <= capacity());
  strcpy(m_arBuffer + len, pString);
  return *this;
}
  
//==============================================================================
template  <size_t N>
inline
HardString<N>& HardString<N>::operator +=(const HardString<N>& str)
{
  return (*this += str.c_str());
}
  
//==============================================================================
template  <size_t N>
HardString<N>&  HardString<N>::operator +=(int32_t n)
{
  char  arBuffer[16];
  sprintf(arBuffer, "%d", n);
  XR_ASSERT(HardString<>, strlen(arBuffer) < sizeof(arBuffer));
  return (*this += arBuffer);
}

//==============================================================================
template  <size_t N>
inline
HardString<N> HardString<N>::operator +(const char* pString) const
{
  HardString<N> str(*this);
  str += pString;
  return str;
}

//==============================================================================
template  <size_t N>
inline
HardString<N> HardString<N>::operator +(const HardString<N>& str) const
{
  return (*this + str.c_str());
}
  
//==============================================================================
template  <size_t N>
inline
HardString<N> HardString<N>::operator +(int32_t n) const
{
  HardString<N> str(*this);
  return str += n;
}

//==============================================================================
template  <size_t N>
inline
const char&  HardString<N>::operator[](size_t i) const
{
  XR_ASSERT(HardString<>, i >= 0);
  XR_ASSERT(HardString<>, i < size());
  return m_arBuffer[i];
}

//==============================================================================
template  <size_t N>
inline
char&  HardString<N>::operator[](size_t i)
{
  XR_ASSERT(HardString<>, i >= 0);
  XR_ASSERT(HardString<>, i < size());
  return m_arBuffer[i];
}
  
//==============================================================================
template  <size_t N>
inline
bool  HardString<N>::operator==(const char* pString) const
{
  return strcmp(m_arBuffer, pString) == 0;
}

//==============================================================================
template  <size_t N>
inline
bool  HardString<N>::operator==(const HardString<N>& str) const
{
  return *this == str.c_str();
}

//==============================================================================
template  <size_t N>
inline
bool  HardString<N>::operator<(const char* pString) const
{
  return strcmp(m_arBuffer, pString) < 0;
}

//==============================================================================
template  <size_t N>
inline
bool  HardString<N>::operator<(const HardString<N>& str) const
{
  return *this < str.c_str();
}

//==============================================================================
template  <size_t N>
inline
bool  operator!=(HardString<N>& s, const char* pStr)
{
  return !(s == pStr);
}

//==============================================================================
template  <size_t N>
inline
bool  operator!=(HardString<N>& s0, HardString<N>& s1)
{
  return !(s0 == s1);
}

//==============================================================================
template  <size_t N>
inline
bool  operator==(const char* pStr, HardString<N>& s)
{
  return s == pStr;
}

//==============================================================================
template  <size_t N>
inline
bool  operator!=(const char* pStr, HardString<N>& s)
{
  return !(s == pStr);
}

//==============================================================================
template  <size_t N>
inline
bool  operator<(const char* pStr, HardString<N>& s)
{
  return strcmp(pStr, s.c_str()) < 0;
}

} // XR

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // XR_HARDSTRING_HPP
//
// Nuclear Heart Games
// XRhodes
//
// HardString.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    09/30/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_HARDSTRING_HPP
#define XR_HARDSTRING_HPP

#include  <string.h>
#include  <ctype.h>

namespace XR
{

//==============================================================================
///@brief Fixed capacity string class that doesn't allocate memory through its
/// lifespan.
template  <int N>
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
  explicit HardString(int32 n);
  
  // general
  int             size() const;
  int             capacity() const;
  const char*     c_str() const;
  int             find(const char* pSub) const;
  int             find(int c) const;
  int             rfind(int c) const;

  HardString<N>&  tolower();
  HardString<N>&  toupper();

  void            clear();
  HardString<N>&  assign(const char* p, int len);

  // operators
  HardString<N>&  operator =(const char* pString);
  HardString<N>&  operator =(const HardString<N>& str);
  HardString<N>&  operator =(int n);

  HardString<N>&  operator +=(const char* pString);
  HardString<N>&  operator +=(const HardString<N>& str);
  HardString<N>&  operator +=(int n);

  HardString<N>   operator +(const char* pString) const;
  HardString<N>   operator +(const HardString<N>& str) const;
  HardString<N>   operator +(int n) const;

  char            operator[](int i) const;
  char&           operator[](int i);
  
  bool            operator==(const char* pString) const;
  bool            operator==(const HardString<N>& str) const;

                  operator const char*() const;

protected:
  // data
  char  m_arBuffer[N];
};

template  <>
class HardString<0>
{};

//==============================================================================
/*
  implementation
*/
//==============================================================================
template  <int N>
HardString<N>::HardString()
: m_arBuffer()
{
  m_arBuffer[0] = 0;
}
  
//==============================================================================
template  <int N>
HardString<N>::HardString(const char* pString)
: m_arBuffer()
{
  XR_ASSERT(HardString<>, strlen(pString) < N);
  strcpy(m_arBuffer, pString);    
}

//==============================================================================
template  <int N>
HardString<N>::HardString(int32 n)
: m_arBuffer()
{
  operator+=(n);
}
  
//==============================================================================
template  <int N>
int HardString<N>::size() const
{
  return strlen(m_arBuffer);
}
  
//==============================================================================
template  <int N>
int HardString<N>::capacity() const
{
  return kCapacity;
}

//==============================================================================
template  <int N>
const char* HardString<N>::c_str() const
{
  return m_arBuffer;
}

//==============================================================================
template  <int N>
int HardString<N>::find(const char* pSub) const
{
  char* p(strstr(m_arBuffer, pSub));
  return p != 0 ? static_cast<int>(p - m_arBuffer) : kCapacity;
}

//==============================================================================
template  <int N>
int HardString<N>::find(int c) const
{
  char* p(strchr(m_arBuffer, c));
  return p != 0 ? static_cast<int>(p - m_arBuffer) : kCapacity;
}

//==============================================================================
template  <int N>
int HardString<N>::rfind(int c) const
{
  char* p(strrchr(m_arBuffer, c));
  return p != 0 ? static_cast<int>(p - m_arBuffer) : kCapacity;
}

//==============================================================================
template  <int N>
HardString<N>&  HardString<N>::tolower()
{
  int n(size());
  for(int i = 0; i < n; ++i)
  {
    m_arBuffer[i] = ::tolower(m_arBuffer[i]);
  }
  return *this;
}

//==============================================================================
template  <int N>
HardString<N>&  HardString<N>::toupper()
{
  int n(size());
  for(int i = 0; i < n; ++i)
  {
    m_arBuffer[i] = ::toupper(m_arBuffer[i]);
  }
  return *this;
}

//==============================================================================
template  <int N>
void  HardString<N>::clear()
{
  m_arBuffer[0] = 0;
}

//==============================================================================
template  <int N>
HardString<N>&  HardString<N>::assign(const char* pStr, int size)
{
  XR_ASSERT(HardString<N>, pStr != 0);
  int max(capacity());
  if(size > max)
  {
    size = max;
  }
  memcpy(m_arBuffer, pStr, max);
  m_arBuffer[max] = 0;
  return *this;
}

//==============================================================================
template  <int N>
HardString<N>& HardString<N>::operator =(const char* pString)
{
  XR_ASSERT(HardString<N>, strlen(pString) < kCapacity);
  strcpy(m_arBuffer, pString);
  return *this;
}

//==============================================================================
template  <int N>
HardString<N>& HardString<N>::operator =(const HardString<N>& str)
{
  return (*this = str.c_str());
}

//==============================================================================
template  <int N>
HardString<N>& HardString<N>::operator =(int n)
{
  char  arBuffer[16];
  sprintf(arBuffer, "%d", n);
  XR_ASSERT(HardString<>, strlen(arBuffer) < sizeof(arBuffer));
  return (*this = arBuffer);
}

//==============================================================================
template  <int N>
HardString<N>& HardString<N>::operator +=(const char* pString)
{
  XR_ASSERT(HardString<>, size() + strlen(pString) < N);
  sprintf(m_arBuffer + size(), pString);
  return *this;
}
  
//==============================================================================
template  <int N>
HardString<N>& HardString<N>::operator +=(const HardString<N>& str)
{
  return (*this += str.c_str());
}
  
//==============================================================================
template  <int N>
HardString<N>&  HardString<N>::operator +=(int n)
{
  char  arBuffer[16];
  sprintf(arBuffer, "%d", n);
  XR_ASSERT(HardString<>, strlen(arBuffer) < sizeof(arBuffer));
  return (*this += arBuffer);
}

//==============================================================================
template  <int N>
HardString<N> HardString<N>::operator +(const char* pString) const
{
  HardString<N> str(*this);
  str += pString;
  return str;
}

//==============================================================================
template  <int N>
HardString<N> HardString<N>::operator +(const HardString<N>& str) const
{
  return (*this + str.c_str());
}
  
//==============================================================================
template  <int N>
HardString<N> HardString<N>::operator +(int n) const
{
  HardString<N> str(*this);
  return str += n;
}

//==============================================================================
template  <int N>
char  HardString<N>::operator[](int i) const
{
  XR_ASSERT(HardString<>, i >= 0);
  XR_ASSERT(HardString<>, i < size());
  return m_arBuffer[i];
}

//==============================================================================
template  <int N>
char&  HardString<N>::operator[](int i)
{
  XR_ASSERT(HardString<>, i >= 0);
  XR_ASSERT(HardString<>, i < size());
  return m_arBuffer[i];
}
  
//==============================================================================
template  <int N>
bool  HardString<N>::operator==(const char* pString) const
{
  return strcmp(m_arBuffer, pString) != 0;
}

//==============================================================================
template  <int N>
bool  HardString<N>::operator==(const HardString<N>& str) const
{
  return strcmp(m_arBuffer, str.c_str()) != 0;
}

} // XR

#endif // XR_HARDSTRING_HPP
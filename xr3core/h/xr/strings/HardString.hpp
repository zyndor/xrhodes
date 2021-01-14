#ifndef XR_HARDSTRING_HPP
#define XR_HARDSTRING_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <algorithm>
#include <cstring>
#include <ctype.h>
#include "xr/debug.hpp"

namespace xr
{

namespace detail
{
class HardStringCore
{
protected:// data
  size_t m_size = 0;
};

};

//==============================================================================
///@brief Fixed capacity string class that doesn't allocate memory through its
/// lifespan.
template  <size_t N>
class HardString: protected detail::HardStringCore
{
public:
  // types
  enum
  {
    kCapacity = N - 1
  };

  // structors
  HardString();
  HardString(const char* cs);
  HardString(const char* cs, size_t size);
  explicit HardString(int32_t n);

  // general
  bool            empty() const;
  size_t          size() const;
  size_t          capacity() const;
  const char*     c_str() const;
  char*           data();

  const char*     find(const char* cs) const;
  char*           find(const char* cs);
  const char*     find(int32_t c) const;
  char*           find(int32_t c);

  const char*     rfind(const char* cs) const;
  char*           rfind(const char* cs);
  const char*     rfind(int32_t c) const;
  char*           rfind(int32_t c);

  HardString<N>   substr(size_t offs, size_t size) const;

  HardString<N>&  tolower();
  HardString<N>&  toupper();

  void            clear();
  HardString<N>&  assign(char const* cs, size_t len);
  HardString<N>&  append(char const* cs, size_t len);

  ///@brief Updates the cached size of the HardString. Useful, and should be
  /// used, after directly manipulating the buffer, i.e. data().
  void            UpdateSize();

  // operators
  HardString<N>&  operator =(const char* cs);
  HardString<N>&  operator =(const HardString<N>& hs);
  HardString<N>&  operator =(int32_t n);

  HardString<N>&  operator +=(const char* cs);
  HardString<N>&  operator +=(const HardString<N>& hs);
  HardString<N>&  operator +=(int32_t n);

  HardString<N>   operator +(const char* cs) const;
  HardString<N>   operator +(const HardString<N>& hs) const;
  HardString<N>   operator +(int32_t n) const;

  const char&     operator[](size_t i) const;
  char&           operator[](size_t i);

protected:
  // data
  char   m_buffer[N];
};

template  <>
class HardString<0>
{};

// operator==
template  <size_t N>
bool  operator==(HardString<N> const& s, char const* cs);

template  <size_t N>
bool  operator==(char const* cs, HardString<N> const& s);

template  <size_t N, size_t M>
bool  operator==(HardString<N> const& hs0, HardString<M> const& hs1);

// operator!=
template  <size_t N>
bool  operator!=(HardString<N> const& s, char const* cs);

template  <size_t N>
bool  operator!=(char const* cs, HardString<N> const& s);

template  <size_t N, size_t M>
bool  operator!=(HardString<N> const& hs0, HardString<M> const& hs1);

// operator<
template  <size_t N>
bool  operator<(HardString<N> const& s, char const* cs);

template  <size_t N>
bool  operator<(char const* cs, HardString<N> const& s);

template  <size_t N, size_t M>
bool  operator<(HardString<N> const& hs0, HardString<M> const& hs1);

//==============================================================================
// implementation
//==============================================================================
inline
char* strrstr(const char* haystack, const char* needle)
{
  const size_t needleSize = strlen(needle);
  const size_t haystackSize = strlen(haystack);
  if (needleSize > haystackSize)
  {
    return nullptr;
  }

  char const* p = haystack + haystackSize - needleSize;
  while(p >= haystack && strncmp(p, needle, needleSize) != 0)
  {
    --p;
  }
  return p < haystack ? nullptr : const_cast<char*>(p);
}

//==============================================================================
template  <size_t N>
inline
HardString<N>::HardString()
{
  m_buffer[0] = '\0';
}

//==============================================================================
template  <size_t N>
inline
HardString<N>::HardString(const char* cs)
: HardString<N>(cs, strlen(cs))
{}

//==============================================================================
template  <size_t N>
inline
HardString<N>::HardString(const char* cs, size_t size)
{
  assign(cs, size);
}

//==============================================================================
template  <size_t N>
inline
HardString<N>::HardString(int32_t n)
: HardString<N>()
{
  operator+=(n);
}

//==============================================================================
template <size_t N>
inline
bool HardString<N>::empty() const
{
  return m_size == 0;
}

//==============================================================================
template  <size_t N>
inline
size_t  HardString<N>::size() const
{
  return m_size;
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
  return m_buffer;
}

//==============================================================================
template  <size_t N>
inline
char* HardString<N>::data()
{
  return m_buffer;
}

//==============================================================================
template  <size_t N>
inline
const char* HardString<N>::find(const char* cs) const
{
  return strstr(m_buffer, cs);
}

//==============================================================================
template  <size_t N>
inline
char* HardString<N>::find(const char* cs)
{
  return strstr(m_buffer, cs);
}

//==============================================================================
template  <size_t N>
inline
const char* HardString<N>::find(int32_t c) const
{
  return strchr(m_buffer, c);
}

//==============================================================================
template  <size_t N>
inline
char* HardString<N>::find(int32_t c)
{
  return strchr(m_buffer, c);
}

//==============================================================================
template  <size_t N>
inline
const char* HardString<N>::rfind(const char* cs) const
{
  return strrstr(m_buffer, cs);
}

//==============================================================================
template  <size_t N>
inline
char* HardString<N>::rfind(const char* cs)
{
  return strrstr(m_buffer, cs);
}

//==============================================================================
template  <size_t N>
inline
const char* HardString<N>::rfind(int32_t c) const
{
  return strrchr(m_buffer, c);
}

//==============================================================================
template  <size_t N>
inline
char* HardString<N>::rfind(int32_t c)
{
  return strrchr(m_buffer, c);
}

//==============================================================================
template  <size_t N>
inline
HardString<N> HardString<N>::substr(size_t offs, size_t size) const
{
  XR_ASSERT(HardString, offs + size <= capacity());
  return HardString<N>(m_buffer + offs, size);
}

//==============================================================================
template  <size_t N>
inline
HardString<N>&  HardString<N>::tolower()
{
  std::transform(m_buffer, m_buffer + size(), m_buffer, ::tolower);
  return *this;
}

//==============================================================================
template  <size_t N>
inline
HardString<N>&  HardString<N>::toupper()
{
  std::transform(m_buffer, m_buffer + size(), m_buffer, ::toupper);
  return *this;
}

//==============================================================================
template  <size_t N>
inline
void  HardString<N>::clear()
{
  m_buffer[0] = '\0';
  m_size = 0;
}

//==============================================================================
template  <size_t N>
HardString<N>&  HardString<N>::assign(const char* cs, size_t size)
{
  XR_ASSERT(HardString, cs != nullptr);
  XR_ASSERT(HardString, size <= capacity());
  size = std::min(size, capacity());
  std::copy(cs, cs + size, m_buffer);
  m_buffer[size] = '\0';
  m_size = size;
  return *this;
}

//==============================================================================
template  <size_t N>
HardString<N>&  HardString<N>::append(const char* cs, size_t size)
{
  XR_ASSERT(HardString, cs != nullptr);
  XR_ASSERT(HardString, m_size + size <= capacity());
  size = std::min(size, capacity() - m_size);
  std::copy(cs, cs + size, m_buffer + m_size);
  size += m_size;
  m_buffer[size] = '\0';
  m_size = size;
  return *this;
}

//==============================================================================
template<size_t N>
inline void HardString<N>::UpdateSize()
{
  const auto size = strlen(m_buffer);
  XR_ASSERT(HardString, size <= kCapacity);
  m_size = size;
}

//==============================================================================
template  <size_t N>
inline
HardString<N>& HardString<N>::operator =(char const* cs)
{
  return assign(cs, strlen(cs));
}

//==============================================================================
template  <size_t N>
inline
HardString<N>& HardString<N>::operator =(HardString<N> const& hs)
{
  XR_ASSERT(HardString, hs.size() <= kCapacity);
  return assign(hs.m_buffer, hs.m_size);
}

//==============================================================================
template  <size_t N>
HardString<N>& HardString<N>::operator =(int32_t n)
{
  char buffer[16];
  const auto size = std::snprintf(buffer, sizeof(buffer), "%d", n);
  XR_ASSERT(HardString, size < sizeof(buffer));
  return assign(buffer, size);
}

//==============================================================================
template  <size_t N>
inline
HardString<N>& HardString<N>::operator +=(const char* cs)
{
  return append(cs, strlen(cs));
}

//==============================================================================
template  <size_t N>
inline
HardString<N>& HardString<N>::operator +=(const HardString<N>& hs)
{
  return append(hs.m_buffer, hs.m_size);
}

//==============================================================================
template  <size_t N>
HardString<N>&  HardString<N>::operator +=(int32_t n)
{
  char buffer[16];
  const auto len = std::snprintf(buffer, sizeof(buffer), "%d", n);
  XR_ASSERT(HardString, len < sizeof(buffer));
  return append(buffer, len);
}

//==============================================================================
template  <size_t N>
inline
HardString<N> HardString<N>::operator +(const char* cs) const
{
  HardString<N> hs(*this);
  hs += cs;
  return hs;
}

//==============================================================================
template  <size_t N>
inline
HardString<N> HardString<N>::operator +(const HardString<N>& hs) const
{
  return (*this + hs.c_str());
}

//==============================================================================
template  <size_t N>
inline
HardString<N> HardString<N>::operator +(int32_t n) const
{
  HardString<N> hs(*this);
  return hs += n;
}

//==============================================================================
template  <size_t N>
inline
const char&  HardString<N>::operator[](size_t i) const
{
  XR_ASSERT(HardString, i >= 0);
  XR_ASSERT(HardString, i < size());
  return m_buffer[i];
}

//==============================================================================
template  <size_t N>
inline
char&  HardString<N>::operator[](size_t i)
{
  XR_ASSERT(HardString, i >= 0);
  XR_ASSERT(HardString, i < size());
  return m_buffer[i];
}

//==============================================================================
template  <size_t N>
inline
bool  operator==(HardString<N> const& s, char const* cs)
{
  return strcmp(s.c_str(), cs) == 0;
}

//==============================================================================
template  <size_t N>
inline
bool  operator==(char const* cs, HardString<N> const& s)
{
  return s == cs;
}

//==============================================================================
template  <size_t N, size_t M>
inline
bool  operator==(HardString<N> const& hs0, HardString<M> const& hs1)
{
  return hs0 == hs1.c_str();
}

//==============================================================================
template  <size_t N>
inline
bool  operator!=(HardString<N> const& s, char const* cs)
{
  return !(s == cs);
}

//==============================================================================
template  <size_t N>
inline
bool  operator!=(char const* cs, HardString<N> const& s)
{
  return s != cs;
}

//==============================================================================
template  <size_t N, size_t M>
inline
bool  operator!=(HardString<N> const& hs0, HardString<M> const& hs1)
{
  return hs0 != hs1.c_str();
}

//==============================================================================
template  <size_t N>
inline
bool  operator<(HardString<N> const& hs, char const* cs)
{
  return strcmp(hs.c_str(), cs) < 0;
}

//==============================================================================
template  <size_t N>
inline
bool  operator<(char const* cs, HardString<N> const& hs)
{
  return strcmp(cs, hs.c_str()) < 0;
}

//==============================================================================
template  <size_t N, size_t M>
inline
bool  operator<(HardString<N> const& hs0, HardString<M> const& hs1)
{
  return hs0 < hs1.c_str();
}

} // xr

#endif // XR_HARDSTRING_HPP

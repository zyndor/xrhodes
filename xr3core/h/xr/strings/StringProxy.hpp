#ifndef XR_STRINGPROXY_HPP
#define XR_STRINGPROXY_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include <cstring>

namespace xr
{

//==============================================================================
///@brief StringProxy offers capability to reference a const char buffer
/// without having to allocate memory and copy contents, until absolutely
/// needed -- when the StringProxy object itself is copied.
class StringProxy
{
public:
  // structors
  StringProxy()
  : m_data(nullptr),
    m_isOwned(false)
  {}

  StringProxy(char const* d, bool copy = false)
  : m_data(d),
    m_isOwned(false)
  {
    if (copy)
    {
      CopyInternal(d);
    }
  }

  StringProxy(StringProxy const& rhs)
  : m_data(nullptr),
    m_isOwned(false)
  {
    CopyInternal(rhs.m_data);
  }

  StringProxy(StringProxy&& rhs)
  : m_data(rhs.m_data),
    m_isOwned(rhs.m_isOwned)
  {
    rhs.m_isOwned = false;
  }

  ~StringProxy()
  {
    Destroy();
  }

  // general
  void Destroy()
  {
    if (m_isOwned)
    {
      delete[] m_data;
      m_data = nullptr;
      m_isOwned = false;
    }
  }

  // operator overloads
  operator char const*() const
  {
    return m_data;
  }

  StringProxy& operator=(StringProxy const& rhs)
  {
    StringProxy temp(rhs);  // perform copy

    Destroy();
    m_data = temp.m_data;
    m_isOwned = temp.m_isOwned;
    temp.m_isOwned = false;

    return *this;
  }

  StringProxy& operator=(char const* d)
  {
    // Dubious why anyone would want to do this, but in case of
    // assigning its own buffer, do nothing. We do not want to make
    // a deep copy at this point, so we have to do this.
    if (d != m_data)
    {
      Destroy(); // clean up previous buffer
      m_data = d; // assign, but don't own buffer.
    }
    return *this;
  }

  // overloads
  bool operator<(StringProxy const& rhs) const
  {
    return strcmp(m_data, rhs.m_data) < 0;
  }

private:
  // data
  char const* m_data;
  bool m_isOwned;

  // internal
  void CopyInternal(char const* d)
  {
    size_t len = strlen(d);
    char* buffer = new char[len + 1]; // null terminator
    memcpy(buffer, d, len);
    buffer[len] = '\0';

    m_data = buffer;
    m_isOwned = true;
  }
};

} // xr

#endif //XR_STRINGPROXY_HPP

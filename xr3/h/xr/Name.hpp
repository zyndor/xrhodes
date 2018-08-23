#ifndef XR_NAME_HPP
#define XR_NAME_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <cstdint>
#include <string>

namespace xr
{

//==============================================================================
class Name
{
public:
  // structors
  Name();
  explicit Name(uint32_t value);
  explicit Name(const char* name, size_t length);
  explicit Name(const std::string& name);
  ~Name();

  // general
#ifdef  XR_DEBUG
  const char* GetDebugValue() const;
#endif  //XR_DEBUG

  // operators
  bool  operator==(const Name& rhs) const;

  Name& operator=(uint32_t value);
  Name& operator=(const std::string& name);

    operator uint32_t() const;

private:
  // data
  uint32_t  m_value;

#ifdef  XR_DEBUG
  std::string m_debugValue;
#endif  //XR_DEBUG
};

//==============================================================================
//  implementation
//==============================================================================
inline
bool  Name::operator==(const Name& rhs) const
{
  return m_value == rhs.m_value;
}

//==============================================================================
inline
  Name::operator uint32_t() const
{
  return m_value;
}

//==============================================================================
#ifdef  XR_DEBUG
inline
const char*  Name::GetDebugValue() const
{
  return m_debugValue.c_str();
}
#endif  //XR_DEBUG

} // xr

#endif // XR_NAME_HPP

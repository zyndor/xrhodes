//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Name.hpp"
#include "xr/Hash.hpp"
#include "xr/debug.hpp"
#include <string.h>

namespace xr
{
namespace
{
#if defined(XR_DEBUG)
const char* const kUndefined = "#UNDEFINED#";
#endif
}

//==============================================================================
Name::Name()
: m_value(0)
#if defined(XR_DEBUG)
  ,
  m_debugValue(kUndefined)
#endif  //XR_DEBUG
{}

//==============================================================================
Name::Name(uint32_t name)
: m_value(name)
#if defined(XR_DEBUG)
  ,
  m_debugValue(kUndefined)
#endif  //XR_DEBUG
{}

//==============================================================================
Name::Name(const char* name, size_t length)
: m_value(Hash::String32(name, length))
#if defined(XR_DEBUG)
  ,
  m_debugValue(name, length)
#endif  //XR_DEBUG
{}

//==============================================================================
Name::Name(const std::string& name)
: m_value(Hash::String32(name.c_str()))
#if defined(XR_DEBUG)
  ,
  m_debugValue(name)
#endif  //XR_DEBUG
{}

//==============================================================================
Name::~Name()
{}

//==============================================================================
Name& Name::operator=(uint32_t value)
{
  m_value = value;
#if defined(XR_DEBUG)
  m_debugValue = kUndefined;
#endif  //XR_DEBUG
  return *this;
}

//==============================================================================
Name& Name::operator=(const std::string& name)
{
  m_value = Hash::String32(name.c_str());
#if defined(XR_DEBUG)
  m_debugValue = name;
#endif  //XR_DEBUG
  return *this;
}

} // xr

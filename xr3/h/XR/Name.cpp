//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Name.hpp"
#include <XR/debug.hpp>
#include <string.h>

namespace XR
{
namespace
{
const char* const kUndefined = "#UNDEFINED#";
}

//==============================================================================
Name::Name()
: m_value(0)
#ifdef  XR_DEBUG
  ,
  m_debugValue(kUndefined)
#endif  //XR_DEBUG
{}

//==============================================================================
Name::Name(uint32_t name)
: m_value(name)
#ifdef  XR_DEBUG
  ,
  m_debugValue(kUndefined)
#endif  //XR_DEBUG
{}

//==============================================================================
Name::Name(const char* pName, size_t length)
  : m_value(XR::Hash::String(pName, length))
#ifdef  XR_DEBUG
  ,
  m_debugValue(pName, length)
#endif  //XR_DEBUG
{}

//==============================================================================
Name::Name(const std::string& name)
: m_value(XR::Hash::String(name.c_str()))
#ifdef  XR_DEBUG
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
#ifdef  XR_DEBUG
  m_debugValue = kUndefined;
#endif  //XR_DEBUG
  return *this;
}

//==============================================================================
Name& Name::operator=(const std::string& name)
{
  m_value = XR::Hash::String(name.c_str());
#ifdef  XR_DEBUG
  m_debugValue = name;
#endif  //XR_DEBUG
  return *this;
}

} // XR

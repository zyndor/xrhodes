//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include <string.h>
#include "Name.hpp"

namespace XR
{
namespace
{
const char* kUndefined = "#UNDEFINED#";
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
Name::Name(uint32 name)
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
Name& Name::operator=(uint32 value)
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

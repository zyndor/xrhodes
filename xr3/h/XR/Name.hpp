//
// Nuclear Heart Games
// XRhodes
//
// Name.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    10/04/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_NAME_HPP
#define XR_NAME_HPP

#include "Hash.hpp"
#include <string>

namespace XR
{

//==============================================================================
class Name
{
public:
  // structors
  Name();
  explicit Name(uint32 value);
  explicit Name(const std::string& name);
  ~Name();
  
  // general
#ifdef  XR_DEBUG
  const char* GetDebugValue() const;
#endif  //XR_DEBUG

  // operators
  bool  operator==(const Name& rhs) const;

  Name& operator=(uint32 value);
  Name& operator=(const std::string& name);

    operator uint32() const;

private:
  // data
  uint32  m_value;

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
  Name::operator uint32() const
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

} // XR

#endif // XR_NAME_HPP

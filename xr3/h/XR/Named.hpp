//
// Nuclear Heart Games
// XRhodes
//
// Named.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    10/04/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_NAMED_HPP
#define XR_NAMED_HPP

#include  "Hash.hpp"

namespace XR
{

//==============================================================================
class Named
{
public:
  // types
  class FindPredicate
  {
  public:
    explicit FindPredicate(uint32 name)
    : m_name(name)
    {}

    bool  operator()(const Named* p) const
    {
      return m_name == p->m_name;
    }

  private:
    // data
    uint32  m_name;
  };

  // static
  static uint32 TransformName(const char* pName, int32 size);
  
  // structors
  Named();
  explicit Named(uint32 name);
  explicit Named(const char* pName);
  ~Named();
  
  // general
  uint32  GetName() const;
  void    SetName(uint32 name);
  void    SetName(const char* pName);
  
private:
  // data
  uint32  m_name;
};

//==============================================================================
//  implementation
//==============================================================================
inline 
uint32 Named::TransformName(const char* pName, int size)
{
  return Hash::String(pName, size);
}

//==============================================================================
inline
uint32  Named::GetName() const
{
  return m_name;
} 

} // XR

#endif // XR_NAMED_HPP

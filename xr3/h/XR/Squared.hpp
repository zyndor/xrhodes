//
// Nuclear Heart Games
// XRhodes
//
// Squared.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    02/05/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#ifndef XR_SQUARED_HPP
#define XR_SQUARED_HPP

namespace XR
{

//==============================================================================
///@brief  Convenience class to store values that are mainly used (i.e. read) as
/// squared.
class Squared
{
public:
  // structors
  Squared(float v = .0f);
    
  // general
  float Sqrt() const;
  void  Set(float v, bool isSquared);
  
  // operators
  float& operator=(float v);
  operator float() const;
  
private:
  // data
  float m_value;
};


//==============================================================================
// implementation
//==============================================================================
inline
Squared::Squared(float v = .0f)
: m_value(v)
{}
    
//==============================================================================
inline
float Squared::Sqrt() const
{
  return  sqrtf(m_value);
}

//==============================================================================
inline
void  Squared::Set(float v, bool isSquared)
{
  XR_ASSERT(Squared, !isSquared || v >= .0f);
  m_value = isSquared ? v : v * v;
}

//==============================================================================
inline
float& Squared::operator=(float v)
{
  XR_ASSERT(Squared, v >= .0f);
  m_value = v;
  return m_value;
}

//==============================================================================
inline
Squared::operator float() const
{
  return m_value;
}

} // XR

#endif // XR_SQUARED_HPP

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_TWEAKBENCH_HPP
#define XR_TWEAKBENCH_HPP

#include  <map>
#include  "types.hpp"

namespace XR
{

//==============================================================================
///@brief Facilitates the live tweaking of variables of primitive types by
/// maintaining a registrar of them and allowing centralised access to them
/// using identifiers.
//==============================================================================
class TweakBench
{
public:
  // types
  enum  { INVALID_ID = -1 };

  struct  Variable
  {
    enum  Type
    {
      T_INT,
      T_FLOAT,
      T_BOOL,
      T_INVALID
    };

    Type  type;
    void* pVariable;
  };

  // structors
  TweakBench();
  ~TweakBench();

  // general
  template  <typename T>
  uint32  Register(uint32 name, T& var);

  bool    Register(uint32 name, float& var);
  bool    Register(uint32 name, int& var);
  bool    Register(uint32 name, bool& var);
  
  template  <typename T>
  bool    Get(uint32 name, T& val) const;

  bool    Get(uint32 name, float& val) const;
  bool    Get(uint32 name, int& val) const;
  bool    Get(uint32 name, bool& val) const;

  template  <typename T>
  bool    Set(uint32 name, T val);

  bool    Set(uint32 name, float val);
  bool    Set(uint32 name, int val);
  bool    Set(uint32 name, bool val);
  
  Variable::Type  GetType(uint32 name) const;

protected:
  // types
  typedef std::map<uint32, Variable>  VariableMap;

  // data
  VariableMap m_variables;
};

//==============================================================================
// implementation
//==============================================================================
template  <typename T>
uint32  TweakBench::Register(uint32 name, T& var)
{
  return Register(name, var) ? name : INVALID_ID;
}

//==============================================================================
template  <typename T>
bool    TweakBench::Get(uint32 name, T& val) const
{
  return Get(name, val);
}

//==============================================================================
template  <typename T>
bool    TweakBench::Set(uint32 name, T val)
{
  return Set(name, val);
}

} // XR

#endif  //XR_TWEAKBENCH_HPP

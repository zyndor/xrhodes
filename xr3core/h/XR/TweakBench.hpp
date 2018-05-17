#ifndef XR_TWEAKBENCH_HPP
#define XR_TWEAKBENCH_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <map>
#include <cstdint>

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
    void* variable;
  };

  // structors
  TweakBench();
  ~TweakBench();

  // general
  template  <typename T>
  uint32_t  Register(uint32_t name, T& var);

  bool    Register(uint32_t name, float& var);
  bool    Register(uint32_t name, int& var);
  bool    Register(uint32_t name, bool& var);

  template  <typename T>
  bool    Get(uint32_t name, T& val) const;

  bool    Get(uint32_t name, float& val) const;
  bool    Get(uint32_t name, int& val) const;
  bool    Get(uint32_t name, bool& val) const;

  template  <typename T>
  bool    Set(uint32_t name, T val);

  bool    Set(uint32_t name, float val);
  bool    Set(uint32_t name, int val);
  bool    Set(uint32_t name, bool val);

  Variable::Type  GetType(uint32_t name) const;

protected:
  // types
  typedef std::map<uint32_t, Variable>  VariableMap;

  // data
  VariableMap m_variables;
};

//==============================================================================
// implementation
//==============================================================================
template  <typename T>
uint32_t  TweakBench::Register(uint32_t name, T& var)
{
  return Register(name, var) ? name : INVALID_ID;
}

//==============================================================================
template  <typename T>
bool    TweakBench::Get(uint32_t name, T& val) const
{
  return Get(name, val);
}

//==============================================================================
template  <typename T>
bool    TweakBench::Set(uint32_t name, T val)
{
  return Set(name, val);
}

} // XR

#endif  //XR_TWEAKBENCH_HPP

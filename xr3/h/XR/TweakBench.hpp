//
// Nuclear Heart Games
// XRhodes
//
// TweakBench.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    17/08/2014
//
// copyright (c) 2011 - 2015. All rights reserved.
// 
//==============================================================================
#if !defined XR_TWEAKBENCH_HPP
#define XR_TWEAKBENCH_HPP

#include  <map>
#include  "Hash.hpp"

namespace XR
{

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
  uint32  Register(const char* pName, T& var);

  bool    Register(uint32 name, float& var);
  bool    Register(uint32 name, int& var);
  bool    Register(uint32 name, bool& var);
  
  template  <typename T>
  bool    Get(const char* pName, T& val) const;

  bool    Get(uint32 name, float& val) const;
  bool    Get(uint32 name, int& val) const;
  bool    Get(uint32 name, bool& val) const;

  template  <typename T>
  bool    Set(const char* pName, T val);

  bool    Set(uint32 name, float val);
  bool    Set(uint32 name, int val);
  bool    Set(uint32 name, bool val);
  
  Variable::Type  GetType(const char* pName) const;
  Variable::Type  GetType(uint32 name) const;

  bool    Load(const char* pFilename);
  bool    Save(const char* pFilename) const;

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
uint32  TweakBench::Register(const char* pName, T& var)
{
  XR_ASSERT(TweakBench, pName != 0);
  uint32  hash(XR::Hash::String(pName));
  return Register(hash, var) ? hash : INVALID_ID;
}

//==============================================================================
template  <typename T>
bool    TweakBench::Get(const char* pName, T& val) const
{
  XR_ASSERT(TweakBench, pName != 0);
  uint32  hash(XR::Hash::String(pName));
  return Get(hash, val);
}

//==============================================================================
template  <typename T>
bool    TweakBench::Set(const char* pName, T val)
{
  XR_ASSERT(TweakBench, pName != 0);
  uint32  hash(XR::Hash::String(pName));
  return Set(hash, val);
}

//==============================================================================
inline
TweakBench::Variable::Type TweakBench::GetType(const char* pName) const
{
  return GetType(XR::Hash::String(pName));
}

} // XR

#endif  //XR_TWEAKBENCH_HPP

#include  "TweakBench.hpp"

namespace XR
{

//==============================================================================
TweakBench::TweakBench()
: m_variables()
{}

//==============================================================================
TweakBench::~TweakBench()
{}

//==============================================================================
bool  TweakBench::Register(uint32 name, float& var)
{
  VariableMap::const_iterator iFind(m_variables.find(name));
  bool  success(iFind == m_variables.end());
  if(success)
  {
    Variable  v = { Variable::T_FLOAT, &var };
    m_variables[name] = v;
  }
  return success;
}

//==============================================================================
bool  TweakBench::Register(uint32 name, int& var)
{
  VariableMap::const_iterator iFind(m_variables.find(name));
  bool  success(iFind == m_variables.end());
  if(success)
  {
    Variable  v = { Variable::T_INT, &var };
    m_variables[name] = v;
  }
  return success;
}

//==============================================================================
bool  TweakBench::Register(uint32 name, bool& var)
{
  VariableMap::const_iterator iFind(m_variables.find(name));
  bool  success(iFind == m_variables.end());
  if(success)
  {
    Variable  v = { Variable::T_BOOL, &var };
    m_variables[name] = v;
  }
  return success;
}

//==============================================================================
bool  TweakBench::Get(uint32 name, float& val) const
{
  VariableMap::const_iterator iFind(m_variables.find(name));
  bool  success(iFind != m_variables.end() &&
          iFind->second.type == Variable::T_FLOAT);
  if(success)
  {
    val = *static_cast<const float*>(iFind->second.pVariable);
  }
  return success;
}

//==============================================================================
bool  TweakBench::Get(uint32 name, int& val) const
{
  VariableMap::const_iterator iFind(m_variables.find(name));
  bool  success(iFind != m_variables.end() &&
          iFind->second.type == Variable::T_INT);
  if(success)
  {
    val = *static_cast<const int*>(iFind->second.pVariable);
  }
  return success;
}

//==============================================================================
bool  TweakBench::Get(uint32 name, bool& val) const
{
  VariableMap::const_iterator iFind(m_variables.find(name));
  bool  success(iFind != m_variables.end() &&
          iFind->second.type == Variable::T_BOOL);
  if(success)
  {
    val = *static_cast<const bool*>(iFind->second.pVariable);
  }
  return success;
}

//==============================================================================
bool  TweakBench::Set(uint32 name, float val)
{
  VariableMap::iterator iFind(m_variables.find(name));
  bool  success(iFind != m_variables.end() &&
          iFind->second.type == Variable::T_FLOAT);
  if(success)
  {
    *static_cast<float*>(iFind->second.pVariable) = val;
  }
  return success;
}

//==============================================================================
bool  TweakBench::Set(uint32 name, int val)
{
  VariableMap::iterator iFind(m_variables.find(name));
  bool  success(iFind != m_variables.end() &&
          iFind->second.type == Variable::T_INT);
  if(success)
  {
    *static_cast<int*>(iFind->second.pVariable) = val;
  }
  return success;
}

//==============================================================================
bool  TweakBench::Set(uint32 name, bool val)
{
  VariableMap::iterator iFind(m_variables.find(name));
  bool  success(iFind != m_variables.end() &&
          iFind->second.type == Variable::T_BOOL);
  if(success)
  {
    *static_cast<bool*>(iFind->second.pVariable) = val;
  }
  return success;
}

//==============================================================================
TweakBench::Variable::Type TweakBench::GetType(uint32 name) const
{
  VariableMap::const_iterator iFind(m_variables.find(name));
  return iFind != m_variables.end() ? iFind->second.type : Variable::T_INVALID;
}

} // XR
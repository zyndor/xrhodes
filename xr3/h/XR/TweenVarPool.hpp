#ifndef XR_TWEENVARPOOL_HPP
#define XR_TWEENVARPOOL_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "TweenVar.hpp"
#include "XR/debug.hpp"

namespace XR
{

//==============================================================================
class TweenVarPoolCore
{
public:
  // types
  struct  VarCore:  public TweenVarCore
  {
    int offset;
  };

  // data
  int       m_numVars;
  VarCore*  m_varPool;

  // structors
  TweenVarPoolCore(int numVars);
  ~TweenVarPoolCore();
};

//==============================================================================
template  <typename T>
class TweenVarPool: protected TweenVarPoolCore
{
public:
  // types
  typedef T Type;

  // structors
  TweenVarPool(int numVars);
  ~TweenVarPool();

  // general
  void  Add(float duration, Tweener::Function function, Type target,
    Type& value, Tweener::Callback onFrame, Tweener::Callback onFinished,
    void* callbackData, Tweener& tweener);

  bool  Remove(Type& v, bool finish);

  void  Clear();

protected:
  // static
  static void OnFrameCallback(void* data);
  static void OnFinishedCallback(void* data);
};

//==============================================================================
// implementation
//==============================================================================
template  <typename T>
TweenVarPool<T>::TweenVarPool( int numVars )
: TweenVarPoolCore(numVars)
{}

//==============================================================================
template  <typename T>
TweenVarPool<T>::~TweenVarPool()
{}

//==============================================================================
template  <typename T>
void  TweenVarPool<T>::Add( float duration, Tweener::Function function,
        Type target, Type& value, Tweener::Callback onFrame,
        Tweener::Callback onFinished, void* callbackData, Tweener& tweener)
{
  int iInsert = 0;
  while (iInsert < m_numVars)
  {
    if (m_varPool[iInsert].value)
    {
      break;
    }
    ++iInsert;
  }

  XR_ASSERTMSG(TweenVarPool, iInsert < m_numVars,
    ("Not enough variables available on TweenVarPool %p, try increasing the maximum (%d).", this, m_numVars));

  VarCore&  var = m_varPool[iInsert];
  var.fValue = float(value);
  var.Tween(&value, onFrame, onFinished, callbackData, tweener);

  tweener.Add(duration, function, float(target),
    m_varPool[iInsert].fValue, OnFrameCallback, OnFinishedCallback,
    &m_varPool[iInsert]);
}

//==============================================================================
template  <typename T>
bool TweenVarPool<T>::Remove( Type& v, bool finish )
{
  bool  success = false;
  for (int i = 0; i < m_numVars; ++i)
  {
    if (m_varPool[i].value == &v)
    {
      success = m_varPool[i].tweener->Remove(m_varPool[i].fValue, true);
      break;
    }
  }
  return success;
}

//==============================================================================
template  <typename T>
void TweenVarPool<T>::Clear()
{
  for (int i = 0; i < m_numVars; ++i)
  {
    if (m_varPool[i].value)
    {
      m_varPool[i].tweener->Remove(m_varPool[i].fValue, false);
      m_varPool[i].Clear();
    }
  }
}

//==============================================================================
template  <typename T>
void TweenVarPool<T>::OnFrameCallback( void* data )
{
  TweenVarCore* variable = static_cast<VarCore*>(data);
  *static_cast<Type*>(variable->value) = static_cast<Type>(variable->fValue);

  TweenVarCore::OnFrameCallback(data);
}

//==============================================================================
template  <typename T>
void TweenVarPool<T>::OnFinishedCallback( void* data )
{
  TweenVarCore* variable = static_cast<VarCore*>(data);
  if (variable->onFinished)
  {
    (*variable->onFinished)(variable->callbackData);
  }

  variable->Clear();
}

} // XR

#endif  //XR_TWEENVARPOOL_HPP

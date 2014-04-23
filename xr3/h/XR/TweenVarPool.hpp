//
// Nuclear Heart Games
// XRhodes
//
// TweenVarPool.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    25/05/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_TWEENVARPOOL_HPP
#define XR_TWEENVARPOOL_HPP

#include  "TweenVar.hpp"

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
  VarCore*  m_parVarBuffer;

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
  void  Add(float duration, Tweener::Function pFunction, Type target,
    Type& value, Tweener::Callback pOnFrameCb, Tweener::Callback pOnFinishedCb,
    void* pCallbackData, Tweener& tweener);

  bool  Remove(Type& v, bool finish);

  void  Clear();

protected:
  // static
  static void OnFrameCallback(void* pData);
  static void OnFinishedCallback(void* pData);
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
void TweenVarPool<T>::Add( float duration, Tweener::Function pFunction, Type target, Type& value, Tweener::Callback pOnFrameCb, Tweener::Callback pOnFinishedCb, void* pCallbackData, Tweener& tweener )
{
  int iInsert(0);
  while(iInsert < m_numVars)
  {
    if(m_parVarBuffer[iInsert].pValue == 0)
    {
      break;
    }
    ++iInsert;
  }

  XR_ASSERTMSG(TweenVarPool, iInsert < m_numVars,
    ("Not enough variables available on TweenVarPool %p, try increasing the maximum (%d).", this, m_numVars));

  VarCore&  var(m_parVarBuffer[iInsert]);
  var.fValue = (float)(value);
  var.Tween(&value, pOnFrameCb, pOnFinishedCb, pCallbackData, &tweener);

  tweener.Add(duration, pFunction, (float)(target),
    m_parVarBuffer[iInsert].fValue, OnFrameCallback, OnFinishedCallback,
    &m_parVarBuffer[iInsert]);
}

//==============================================================================
template  <typename T>
bool TweenVarPool<T>::Remove( Type& v, bool finish )
{
  bool  success(false);
  for(int i = 0; i < m_numVars; ++i)
  {
    if(m_parVarBuffer[i].pValue == &v)
    {
      success = m_parVarBuffer[i].pTweener->Remove(m_parVarBuffer[i].fValue, true);
      break;
    }
  }
  return success;
}

//==============================================================================
template  <typename T>
void TweenVarPool<T>::Clear()
{
  for(int i = 0; i < m_numVars; ++i)
  {
    if(m_parVarBuffer[i].pValue != 0)
    {
      m_parVarBuffer[i].pTweener->Remove(m_parVarBuffer[i].fValue, false);
      m_parVarBuffer[i].Clear();
    }
  }
}

//==============================================================================
template  <typename T>
void TweenVarPool<T>::OnFrameCallback( void* pData )
{
  TweenVarCore* pVar(static_cast<VarCore*>(pData));
  *static_cast<Type*>(pVar->pValue) = static_cast<Type>(pVar->fValue);

  TweenVarCore::OnFrameCallback(pData);
}

//==============================================================================
template  <typename T>
void TweenVarPool<T>::OnFinishedCallback( void* pData )
{
  TweenVarCore* pVar(static_cast<VarCore*>(pData));
  if(pVar->pOnFinishedCb != 0)
  {
    (*pVar->pOnFinishedCb)(pVar->pCallbackData);
  }

  pVar->Clear();
}

} // XR

#endif  //XR_TWEENVARPOOL_HPP
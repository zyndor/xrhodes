//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_TWEENVAR_HPP
#define XR_TWEENVAR_HPP

#include "Tweener.hpp"

namespace XR
{

//==============================================================================
struct  TweenVarCore
{
  // static
  static void OnFrameCallback(void* pData);
  static void OnFinishedCallback(void* pData);

  // data
  void*             pValue;
  float             fValue;
  Tweener::Callback pOnFrameCb;
  Tweener::Callback pOnFinishedCb;
  void*             pCallbackData;
  Tweener*          pTweener;

  // general
  void  Clear();

  void  Tween(void* pValue_, Tweener::Callback pOnFrameCb_,
    Tweener::Callback pOnFinishedCb_, void* pCallbackData_, Tweener& t);
};

//==============================================================================
template  <typename T>
struct  TweenVar: protected  TweenVarCore
{
  // types
  typedef T Type;

  // using
  using TweenVarCore::Clear;

  // static
  static void OnFrameCallback(void* pData);
  
  // general
  void  Tween(float duration, Tweener::Function pFunction, Type target,
    Type& value, Tweener::Callback pOnFrameCb_,
    Tweener::Callback pOnFinishedCb_, void* pCallbackData_, Tweener& t);
};

//==============================================================================
// implementation
//==============================================================================
template  <typename T>
void TweenVar<T>::OnFrameCallback( void* pData )
{
  TweenVar<Type>* pVar(static_cast<TweenVar<Type>*>(pData));
  *static_cast<Type*>(pVar->pValue) = static_cast<Type>(pVar->fValue);

  TweenVarCore::OnFrameCallback(pData);
}

//==============================================================================
template  <typename T>
void TweenVar<T>::Tween( float duration, Tweener::Function pFunction,
  Type target, Type& value, Tweener::Callback pOnFrameCb_,
  Tweener::Callback pOnFinishedCb_, void* pCallbackData_, Tweener& t )
{
  fValue = static_cast<Type>(value);
  TweenVarCore::Tween(&value, pOnFrameCb_, pOnFinishedCb_, pCallbackData_, t);
  t.Add(duration, pFunction, float(target), fValue,
    OnFrameCallback, (pOnFinishedCb_ != 0) ? OnFinishedCallback : 0, this);
}

} // XR

#endif  //XR_TWEENVAR_HPP
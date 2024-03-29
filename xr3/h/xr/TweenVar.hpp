#ifndef XR_TWEENVAR_HPP
#define XR_TWEENVAR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "Tweener.hpp"

namespace xr
{

XR_WARNINGS_PUSH
XR_WARNINGS_IGNORE_DEPRECATION
//==============================================================================
struct [[deprecated("Use xr::Animator.")]] TweenVarCore
{
  // static
  static void OnFrameCallback(void* data);
  static void OnFinishedCallback(void* data);

  // data
  void*             value;
  float             fValue;
  Tweener::Callback onFrame;
  Tweener::Callback onFinished;
  void*             callbackData;
  Tweener*          tweener;

  // general
  void  Clear();

  void  Tween(void* value_, Tweener::Callback onFrame_,
    Tweener::Callback onFinished_, void* callbackData_, Tweener& t);
};

//==============================================================================
template  <typename T>
struct [[deprecated("Use xr::Animator.")]] TweenVar: protected  TweenVarCore
{
  // types
  typedef T Type;

  // using
  using TweenVarCore::Clear;

  // static
  static void OnFrameCallback(void* data);

  // general
  [[deprecated("Use xr::Animator")]]
  void  Tween(float duration, Tweener::Function function, Type target,
    Type& value, Tweener::Callback onFrame_,
    Tweener::Callback onFinished_, void* callbackData_, Tweener& t);
};

//==============================================================================
// implementation
//==============================================================================
template  <typename T>
void TweenVar<T>::OnFrameCallback( void* data )
{
  TweenVar<Type>* variable(static_cast<TweenVar<Type>*>(data));
  *static_cast<Type*>(variable->value) = static_cast<Type>(variable->fValue);

  TweenVarCore::OnFrameCallback(data);
}

//==============================================================================
template  <typename T>
void TweenVar<T>::Tween( float duration, Tweener::Function function,
  Type target, Type& value_, Tweener::Callback onFrame_,
  Tweener::Callback onFinished_, void* callbackData_, Tweener& t )
{
  fValue = static_cast<Type>(value_);
  TweenVarCore::Tween(&value, onFrame_, onFinished_, callbackData_, t);
  t.Add(duration, function, float(target), fValue,
    OnFrameCallback, onFinished_ ? OnFinishedCallback : nullptr, this);
}

XR_WARNINGS_POP

} // xr

#endif  //XR_TWEENVAR_HPP

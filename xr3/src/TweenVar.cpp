//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/TweenVar.hpp"

XR_WARNINGS_IGNORE_DEPRECATION

namespace xr
{

void TweenVarCore::OnFrameCallback( void* data )
{
  TweenVarCore* var = static_cast<TweenVarCore*>(data);
  if (var->onFrame)
  {
    (*var->onFrame)(var->callbackData);
  }
}

void TweenVarCore::OnFinishedCallback( void* data )
{
  TweenVarCore* var = static_cast<TweenVarCore*>(data);
  if (var->onFinished)
  {
    (*var->onFinished)(var->callbackData);
  }
}

void TweenVarCore::Clear()
{
  value = 0;
  onFrame = nullptr;
  onFinished = nullptr;
  tweener = nullptr;
}

void  TweenVarCore::Tween(void* value_, Tweener::Callback onFrameCb_,
        Tweener::Callback onFinishedCb_, void* callbackData_, Tweener& t)
{
  value = value_;
  onFrame = onFrameCb_;
  onFinished = onFinishedCb_;
  callbackData = callbackData_;
  tweener = &t;
}

} // xr

#include "TweenVar.hpp"

namespace XR
{

void TweenVarCore::OnFrameCallback( void* pData )
{
  TweenVarCore* pVar(static_cast<TweenVarCore*>(pData));
  if(pVar->pOnFrameCb != 0)
  {
    (*pVar->pOnFrameCb)(pVar->pCallbackData);
  }
}

void TweenVarCore::OnFinishedCallback( void* pData )
{
  TweenVarCore* pVar(static_cast<TweenVarCore*>(pData));
  if(pVar->pOnFinishedCb != 0)
  {
    (*pVar->pOnFinishedCb)(pVar->pCallbackData);
  }
}

void TweenVarCore::Clear()
{
  pValue = 0;
  pOnFrameCb = 0;
  pOnFinishedCb = 0;
  pTweener = 0;
}

void TweenVarCore::Tween( void* pValue_, Tweener::Callback pOnFrameCb_, Tweener::Callback pOnFinishedCb_, void* pCallbackData_, Tweener& t )
{
  pValue = pValue_;
  pOnFrameCb = pOnFrameCb_;
  pOnFinishedCb = pOnFinishedCb_;
  pCallbackData = pCallbackData_;
  pTweener = &t;
}

} // XR
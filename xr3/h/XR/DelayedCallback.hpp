//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_DELAYEDCALLBACK_HPP
#define XR_DELAYEDCALLBACK_HPP

namespace XR
{

//==============================================================================
struct  DelayedCallback
{
  // types
  typedef void(*Callback)(void* pData);

  // static
  static bool IsActivePredicate(const DelayedCallback& v);

  // data
  float     delay;
  Callback  pCallback;
  void*     pCallbackData;

  // general
  bool  IsActive() const;

  void  Update(float tDelta);
};

//==============================================================================
// implementation
//==============================================================================
bool  DelayedCallback::IsActive() const
{
  return delay > .0f;
}

} // XR

#endif  // XR_DELAYEDCALLBACK_HPP
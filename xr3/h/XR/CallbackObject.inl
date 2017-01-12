//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

// no guard

inline
void  CallbackObject::Call(void* pSystem)
{
  XR_ASSERT(CallbackObject, pCb != 0);
  (*pCb)(pSystem, pCbData);
}

//==============================================================================
inline
void  CallbackObject::CallSafe(void* pSystem)
{
  if (pCb != 0)
  {
    (*pCb)(pSystem, pCbData);
  }
}

//==============================================================================
inline
void  CallbackObject::operator()(void* pSystem)
{
  Call(pSystem);
}


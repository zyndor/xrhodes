//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/Callback.hpp>

namespace XR
{

//==============================================================================
void  CallbackObject::CallList(List& l, void* pSystem)
{
  for (List::iterator i0(l.begin()), i1(l.end()); i0 != i1; ++i0)
  {
    i0->Call(pSystem);
  }
}

//==============================================================================
CallbackObject::CallbackObject()
: pCallback(0),
  pUserData(0)
{}

//==============================================================================
CallbackObject::CallbackObject(Callback pCb_, void* pCbData_)
: pCallback(pCb_),
  pUserData(pCbData_)
{}

//==============================================================================
CallbackObject::~CallbackObject()
{}

//==============================================================================
void  CallbackObject::Set(Callback pCb_, void* pCbData_)
{
  pCallback = pCb_;
  pUserData = pCbData_;
}

} // XR

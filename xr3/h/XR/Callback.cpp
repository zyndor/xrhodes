//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Callback.hpp"

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
: pCb(0),
  pCbData(0)
{}

//==============================================================================
CallbackObject::CallbackObject(Callback pCb_, void* pCbData_)
: pCb(pCb_),
  pCbData(pCbData_)
{}

//==============================================================================
CallbackObject::~CallbackObject()
{}

//==============================================================================
void  CallbackObject::Set(Callback pCb_, void* pCbData_)
{
  pCb = pCb_;
  pCbData = pCbData_;
}

} // XR

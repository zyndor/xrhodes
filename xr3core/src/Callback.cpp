//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/Callback.hpp"

namespace xr
{

//==============================================================================
void  CallbackObject::CallList(List& l, void* systemData)
{
  for (List::iterator i0(l.begin()), i1(l.end()); i0 != i1; ++i0)
  {
    i0->Call(systemData);
  }
}

//==============================================================================
CallbackObject::CallbackObject()
: callback(nullptr),
  userData(nullptr)
{}

//==============================================================================
CallbackObject::CallbackObject(Callback callback_, void* userData_)
: callback(callback_),
  userData(userData_)
{}

//==============================================================================
CallbackObject::~CallbackObject()
{}

//==============================================================================
void  CallbackObject::Set(Callback callback_, void* userData_)
{
  callback = callback_;
  userData = userData_;
}

} // xr

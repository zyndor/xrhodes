//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_CALLBACK_HPP
#define XR_CALLBACK_HPP

#include <list>
#include "types.hpp"

namespace XR
{

//==============================================================================
typedef void(*Callback)(void* pSystem, void* pUser);

#include  "detail/CallbackAdaptor.inl"
#include  "detail/CallbackObject.inl"

} // XR

#endif // XR_CALLBACK_HPP

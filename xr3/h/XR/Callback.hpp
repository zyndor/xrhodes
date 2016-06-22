//
// Nuclear Heart Games
// XRhodes
//
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
//
// copyright (c) 2011 - 2016. All rights reserved.
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

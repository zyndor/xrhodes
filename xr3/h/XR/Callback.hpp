//
// Nuclear Heart Games
// XRhodes
//
// Callback.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    11/10/2013
//
// copyright (c) 2011 - 2015. All rights reserved.
//
//==============================================================================
#if !defined XR_CALLBACK_HPP
#define XR_CALLBACK_HPP

#include <list>
#include "types.hpp"

namespace XR
{

//==============================================================================
typedef void(*Callback)(void* pSystem, void* pUser);

//==============================================================================
class CallbackObject 
{
public:
  // types
  typedef std::list<CallbackObject> List;
  
  // static
  static void CallList(List& l, void* pSystem);

  // data
  Callback  pCb;
  void*     pCbData;
  
  // structors
  CallbackObject();
  CallbackObject(Callback pCb_, void* pCbData_);
  ~CallbackObject();
  
  // general
  void  Set(Callback pCb_, void* pCbData_);
  
  void  Call(void* pSystem);
  void  CallSafe(void* pSystem);

  // operators
  void  operator()(void* pSystem);
};

//==============================================================================
// implementation
//==============================================================================
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

} // XR

#endif // XR_CALLBACK_HPP

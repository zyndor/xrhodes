//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_CALLBACK_HPP
#define XR_CALLBACK_HPP

#include <list>
#include <XR/debug.hpp>

namespace XR
{

//==============================================================================
///@brief Callback type used XR-wide.
///@param @a pSystem is a pointer to data provided by the caller of the
///       callback; look for its description at that point.
///@param @a pUser is a pointer that's provided by the user and passed back to
///       the callback without change.
typedef void(*Callback)(void* pSystem, void* pUser);

//==============================================================================
///@brief Adapts the member function of a type C, optionally taking an argument
///       of type T, to XR::Callback. The pUser parameter of the callback is
///       treated as the object (of type C) on which to call the given Method,
///       passing the pSystem parameter, cast to T* or T&. 
template  <typename T = void>
struct CallbackAdaptor
{
  template <class C, void(C::*Method)(T*)>
  static void  Function(void* pSystem, void* pUser)
  {
    XR_ASSERT(CallbackAdaptor, pUser != 0);
    (static_cast<C*>(pUser)->*Method)(static_cast<T*>(pSystem));
  }
};

template  <typename T>
struct CallbackAdaptor<T&>
{
  template <class C, void(C::*Method)(T&)>
  static void  Function(void* pSystem, void* pUser)
  {
    XR_ASSERT(CallbackAdaptor, pUser != 0);
    (static_cast<C*>(pUser)->*Method)(*static_cast<T*>(pSystem));
  }
};

template  <>
struct CallbackAdaptor<void>
{
  template <class C, void(C::*Method)()>
  static void  Function(void* pSystem, void* pUser)
  {
    XR_ASSERT(CallbackAdaptor, pUser != 0);
    (static_cast<C*>(pUser)->*Method)();
  }
};

//==============================================================================
class CallbackObject
{
public:
  // types
  typedef std::list<CallbackObject> List;

  // static
  ///@brief Calls a list of CallbackObjects @a l with the given @a pSystem data.
  static void CallList(List& l, void* pSystem);

  // data
  Callback  pCallback;
  void*     pUserData;  // no ownership

  // structors
  CallbackObject();
  CallbackObject(Callback pCb_, void* pCbData_);
  ~CallbackObject();

  // general
  ///@brief Sets the callback and the user data at once.
  void  Set(Callback pCb_, void* pCbData_);

  ///@brief Calls the callback with the given system data.
  void  Call(void* pSystem);

  ///@brief Calls the callback with the given system data only
  /// if the callback is set.
  void  CallSafe(void* pSystem);

  // operators
  ///@brief Shorthand for Call().
  void  operator()(void* pSystem);

  bool  operator==(CallbackObject const& rhs) const;
};

//==============================================================================
// inline
//==============================================================================
inline
void  CallbackObject::Call(void* pSystem)
{
  XR_ASSERT(CallbackObject, pCallback);
  (*pCallback)(pSystem, pUserData);
}

//==============================================================================
inline
void  CallbackObject::CallSafe(void* pSystem)
{
  if (pCallback)
  {
    (*pCallback)(pSystem, pUserData);
  }
}

//==============================================================================
inline
void  CallbackObject::operator()(void* pSystem)
{
  Call(pSystem);
}

//==============================================================================
inline
bool  CallbackObject::operator==(CallbackObject const& rhs) const
{
  return pCallback == rhs.pCallback && pUserData == rhs.pUserData;
}


} // XR

#endif // XR_CALLBACK_HPP

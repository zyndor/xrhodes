#ifndef XR_CALLBACK_HPP
#define XR_CALLBACK_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "XR/debug.hpp"
#include <list>

namespace XR
{

//==============================================================================
///@brief Callback type used XR-wide.
///@param @a systemData is a pointer to data provided by the caller of the
///       callback; look for its description at that point.
///@param @a pUser is a pointer that's provided by the user and passed back to
///       the callback without change.
typedef void(*Callback)(void* systemData, void* pUser);

//==============================================================================
///@brief Adapts the member function of a type C, optionally taking an argument
///       of type T, to XR::Callback. The pUser parameter of the callback is
///       treated as the object (of type C) on which to call the given Method,
///       passing the systemData parameter, cast to T* or T&.
template  <typename T = void>
struct CallbackAdaptor
{
  template <class C, void(C::*Method)(T*)>
  inline
  static void  Function(void* systemData, void* pUser)
  {
    XR_ASSERT(CallbackAdaptor, pUser != 0);
    (static_cast<C*>(pUser)->*Method)(static_cast<T*>(systemData));
  }

  template <void(*Method)(T*)>
  inline
  static void Function(void* systemData, void*)
  {
    (*Method)(static_cast<T*>(systemData));
  }
};

template  <typename T>
struct CallbackAdaptor<T&>
{
  template <class C, void(C::*Method)(T&)>
  inline
  static void  Function(void* systemData, void* pUser)
  {
    XR_ASSERT(CallbackAdaptor, pUser != 0);
    (static_cast<C*>(pUser)->*Method)(*static_cast<T*>(systemData));
  }

  template <void(*Method)(T&)>
  inline
  static void Function(void* systemData, void*)
  {
    (*Method)(*static_cast<T*>(systemData));
  }
};

template  <>
struct CallbackAdaptor<void>
{
  template <class C, void(C::*Method)()>
  inline
  static void  Function(void* systemData, void* pUser)
  {
    XR_ASSERT(CallbackAdaptor, pUser != 0);
    (static_cast<C*>(pUser)->*Method)();
  }

  template <void(*Method)()>
  inline
  static void Function(void* systemData, void*)
  {
    (*Method)();
  }
};

//==============================================================================
class CallbackObject
{
public:
  // types
  typedef std::list<CallbackObject> List;

  // static
  ///@brief Calls a list of CallbackObjects @a l with the given @a systemData data.
  static void CallList(List& l, void* system);

  // data
  Callback  callback;
  void*     userData;  // no ownership

  // structors
  CallbackObject();
  CallbackObject(Callback callback, void* userData);
  ~CallbackObject();

  // general
  ///@brief Sets the callback and the user data at once.
  void  Set(Callback callback, void* userData);

  ///@brief Calls the callback with the given system data.
  void  Call(void* systemData);

  ///@brief Calls the callback with the given system data only
  /// if the callback is set.
  void  CallSafe(void* systemData);

  // operators
  ///@brief Shorthand for Call().
  void  operator()(void* systemData);

  bool  operator==(CallbackObject const& rhs) const;
};

//==============================================================================
// inline
//==============================================================================
inline
void  CallbackObject::Call(void* systemData)
{
  XR_ASSERT(CallbackObject, callback);
  (*callback)(systemData, userData);
}

//==============================================================================
inline
void  CallbackObject::CallSafe(void* systemData)
{
  if (callback)
  {
    (*callback)(systemData, userData);
  }
}

//==============================================================================
inline
void  CallbackObject::operator()(void* systemData)
{
  Call(systemData);
}

//==============================================================================
inline
bool  CallbackObject::operator==(CallbackObject const& rhs) const
{
  return callback == rhs.callback && userData == rhs.userData;
}


} // XR

#endif // XR_CALLBACK_HPP

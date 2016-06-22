//
// Nuclear Heart Games
// XRhodes
//
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
//
// copyright (c) 2011 - 2016. All rights reserved.
//
//==============================================================================

// no guard

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

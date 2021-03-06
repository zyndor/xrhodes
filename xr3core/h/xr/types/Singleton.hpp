#ifndef XR_SINGLETON_HPP
#define XR_SINGLETON_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/debug.hpp"

namespace xr
{

//==============================================================================
///@brief Singleton template that ensures that no more than one instance of its
/// template type exists.
///@usage Derive your Type from Singleton<Type>.
template  <class T>
class Singleton
{
public:
  // typedef
  typedef T Type;

  // static
  ///@return A pointer to the instance, if any.
  ///@note This will not create an instance.
  static Type* Instance(); // no ownership transfer

  [[deprecated("Use Instance()")]]
  static Type* GetInst()
  {
    return Instance();
  }

protected:
  // structors
  Singleton();
  ~Singleton();

private:
  // static
  static Type*  s_pInstance;
};

//==============================================================================
// implementation
//==============================================================================
template  <class T>
T* Singleton<T>::s_pInstance = nullptr;

//==============================================================================
template  <class T>
T* Singleton<T>::Instance()
{
  return s_pInstance;
}

//==============================================================================
template  <class T>
Singleton<T>::Singleton()
{
  XR_ASSERTMSG(Singleton, s_pInstance == nullptr,
    ("Singleton<> has already been instantiated for this type"));
  s_pInstance = static_cast<Type*>(this);
}

//==============================================================================
template  <class T>
Singleton<T>::~Singleton()
{
  s_pInstance = nullptr;
}

}
#endif //XR_SINGLETON_HPP

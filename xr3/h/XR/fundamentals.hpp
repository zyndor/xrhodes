//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_FUNDAMENTALS_HPP
#define XR_FUNDAMENTALS_HPP

//==============================================================================
///@brief Declares a non-object class, a container for all-static members.
#define XR_NONOBJECT_DECL(className) \
  private:\
    className() = delete;\
    ~className() = delete;

//==============================================================================
///@brief Declares a non-copyable class.
#define XR_NONCOPY_DECL(className) \
  private:\
    className(const className& rhs) = delete;\
    className& operator=(const className& rhs) = delete;

namespace XR
{

//==============================================================================
///@brief Invalid ID to use with XRhodes (after subtracting one from the
/// first valid index to an array).
enum  { INVALID_ID = -1 };

//==============================================================================
///@brief
template  <class T>
class Singleton
{
public:
  // typedef
  typedef T Type;

  // static
  static Type* GetInst();

  // structors
  Singleton();
  ~Singleton();

private:
  // static
  static Type*  s_pInstance;
};

//==============================================================================
///@brief
template  <typename T>
class InstCounted
{
public:
  // static
  static int  GetInstanceCount();

  // structors
  InstCounted();

private:
  // static
  static int  s_instanceCount;
};

//==============================================================================
// implementation
//==============================================================================
template  <class T>
T* Singleton<T>::s_pInstance = 0;

//==============================================================================
template  <class T>
T* Singleton<T>::GetInst()
{
  return s_pInstance;
}

//==============================================================================
template  <class T>
Singleton<T>::Singleton()
{
  XR_ASSERTMSG(Singleton, s_pInstance == 0,
    ("Singleton<> has already been instantiated for thia type"));
  s_pInstance = static_cast<Type*>(this);
}

//==============================================================================
template  <class T>
Singleton<T>::~Singleton()
{
  s_pInstance = 0;
}

//==============================================================================
template  <typename T>
int InstCounted<T>::s_instanceCount = 0;

//==============================================================================
template  <typename T>
int InstCounted<T>::GetInstanceCount()
{
  return s_instanceCount;
}

//==============================================================================
template  <typename T>
InstCounted<T>::InstCounted()
{
  ++s_instanceCount;
}

}  // XR

#endif  // XR_FUNDAMENTALS_HPP

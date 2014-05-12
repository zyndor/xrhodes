//
// Nuclear Heart Games
// XRhodes
//
// fundamentals.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    15/06/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_FUNDAMENTALS_HPP
#define XR_FUNDAMENTALS_HPP

#include "types.hpp"

namespace XR
{

//==============================================================================
///@brief Invalid ID to use with XRhodes (after subtracting one from the
/// first valid index to an array).
enum  { INVALID_ID = -1 };

//==============================================================================
///@brief The Dependent template allows to navigate a hierarchy of classes
/// upwards. E.g. to get from a Room to a Level to a World to a Game.
template  <class Parent>
class  Dependent
{
public:
  // types
  typedef Parent  ParentType;
  
  // structors
  Dependent();
  explicit Dependent(Parent* p);
  ~Dependent();

  // general use
  Parent*  GetParent();

  void  SetParent(Parent* p);

private:
  // data
  Parent* m_pParent;
};

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
template  <class Parent>
Dependent<Parent>::Dependent()
: m_pParent(0)
{}

//==============================================================================
template  <class Parent>
Dependent<Parent>::Dependent(Parent* p)
: m_pParent(p)
{}

//==============================================================================
template  <class Parent>
Dependent<Parent>::~Dependent()
{}

//==============================================================================
///@return  The object's parent.
template  <class Parent>
inline
Parent*  Dependent<Parent>::GetParent()
{
  return m_pParent;
}

//==============================================================================
///@brief Sets @a p as the object's parent.
template  <class Parent>
void  Dependent<Parent>::SetParent(Parent* p)
{
  m_pParent = p;
}

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

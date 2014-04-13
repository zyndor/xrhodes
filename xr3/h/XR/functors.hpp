//
// Nuclear Heart Games
// XRhodes
//
// functors.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    15/06/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_FUNCTORS_HPP
#define XR_FUNCTORS_HPP

#include <algorithm>
#include <functional>
#include <iostream>
#include "typeutils.hpp"

namespace XR
{

//==============================================================================
template  <typename T>
class  ValueFunctorCore
{
protected:
  // types
  typedef T  Type;
  
  // structors
  explicit ValueFunctorCore(T value)
  : m_value(value)
  {}
  
  ///@brief Sets the underlying value to @a d.
  void  Set(T d)
  {
    m_value = d;
  }

  // data
  T  m_value;
};

//==============================================================================
///@brief Sends data down the std::ostream @a pTarget that you have specified
/// on construction.
template  <typename T>
struct  OStreamer: protected ValueFunctorCore<std::ostream*>
{
  typedef ValueFunctorCore<std::ostream*> BaseType;

  OStreamer(std::ostream* pTarget, char delimiter)
  : BaseType(pTarget),
    m_delimiter(delimiter)
  {}

  void  operator()(T t)
  {
    (*BaseType::m_value) << t << m_delimiter;
  }
  
protected:
  char  m_delimiter;
};

//==============================================================================
///@brief Assigns its base value to everything you pass to it.
template  <typename T>
struct  Assigner: protected ValueFunctorCore<T>
{
  typedef ValueFunctorCore<T> BaseType;

  explicit Assigner(T value)
  : ValueFunctorCore<T>(value)
  {}

  inline
  void  operator()(T& d) const
  {
    d = BaseType::m_value;
  }
};

//==============================================================================
///@brief Adds its base value to everything you pass to it.
template  <typename T>
struct  Adder: protected ValueFunctorCore<T>
{
  typedef ValueFunctorCore<T> BaseType;
  
  explicit Adder(T value)
  : BaseType(value)
  {}

  inline
  void  operator()(T& d) const
  {
    d += BaseType::m_value;
  }
};

//==============================================================================
///@brief Multiplies everything you pass to it by its base value.
template  <typename T>
struct  Scaler: protected ValueFunctorCore<T>
{
  typedef ValueFunctorCore<T>  BaseType;

  explicit Scaler(T value)
  : BaseType(value)
  {}

  inline
  void  operator()(T& d)
  {
    d *= BaseType::m_value;
  }
};

//==============================================================================
///@brief Stores the smallest of all values passed to it. Uses operator< for
/// comparison.
template  <typename T>
class Minimizer:  protected ValueFunctorCore<T>
{
public:
  typedef ValueFunctorCore<T> BaseType;
  
  using BaseType::Set;
  
  explicit Minimizer(T d)
  : ValueFunctorCore<T>(d)
  {}
  
  ///@return  The current smallest of all values passed to this Minimizer.
  T  Evaluate() const
  {
    return BaseType::m_value;
  }
  
  void  operator()(T d)
  {
    if (d < BaseType::m_value)
    {
      BaseType::m_value = d;
    }
  }
};

//==============================================================================
///@brief Stores the largest of all values passed to it. Uses operator> for
/// comparison.
template  <typename T>
class Maximizer:  protected ValueFunctorCore<T>
{
public:
  typedef ValueFunctorCore<T> BaseType;

  using BaseType::Set;

  explicit Maximizer(T d)
  : ValueFunctorCore<T>(d)
  {}
  
  ///@return  The current largest of all values passed to this Maximizer.
  T  Evaluate() const
  {
    return BaseType::m_value;
  }

  void  operator()(T d)
  {
    if (d > BaseType::m_value)
    {
      BaseType::m_value = d;
    }
  }
};

//==============================================================================
///@brief Deletes pointers to objects of type @a T.
template  <typename Type>
inline
void  Deleter(Type* p)
{
  delete p;
}

//==============================================================================
///@brief Deletes pointers to objects of type @a T, then sets
/// the pointer to 0.
template  <typename Type>
inline
void  Annihilator(Type*& p)
{
  Deleter(p);
  p = 0;
}

//==============================================================================
///@brief Passes values of type @a ArgType to the a method of an object of
/// type @a CalleeType that is of signature
/// @a ReturnType(@a CalleeType::*)(@a ArgType) [const].
template  <class Callee, typename Return, typename Arg = void>
struct  Passer: ValueFunctorCore<Callee&>
{
  typedef ValueFunctorCore<Callee&> BaseType;

  typedef typename TypeSelect<IsConst<Callee>::kResult,
    Return(Callee::*)(Arg) const,
    Return(Callee::*)(Arg)>::Type Method;

  Passer(Callee& o, Method pm)
  : BaseType(o),
    m_pMethod(pm)
  {}

  Return  operator()(Arg o)
  {
    return (BaseType::m_value.*m_pMethod)(o);
  }

  Method  m_pMethod;
};

//==============================================================================
///@brief Calls member methods of signature
/// @a ReturnType(@a T::*)(@a ArgType) [const] on @a T objects that are
/// passed to it.
template  <class Callee, typename Return = void, typename Arg = void>
struct  Caller
{
  typedef typename TypeSelect<IsConst<Callee>::kResult,
    Return(Callee::*)(Arg)const,
    Return(Callee::*)(Arg)>::Type Method;

  Caller(Method pm, Arg a)
  : m_pMethod(pm),
    m_arg(a)
  {}

  Return  operator()(Callee& obj) const
  {
    return (obj.*m_pMethod)(m_arg);
  }

  Return operator()(Callee* pObj) const
  {
    return (pObj->*m_pMethod)(m_arg);
  }

  Method  m_pMethod;
  Arg m_arg;
};

//==============================================================================
template  <class Callee, typename Return>
struct  Caller<Callee, Return, void>
{
  typedef typename TypeSelect<IsConst<Callee>::kResult,
    Return(Callee::*)()const,
    Return(Callee::*)()>::Type Method;

  explicit Caller(Method pm)
  : m_pMethod(pm)
  {}

  Return  operator()(Callee& obj) const
  {
    return (obj.*m_pMethod)();
  }

  Return operator()(Callee* pObj) const
  {
    return (pObj->*m_pMethod)();
  }

  Method  m_pMethod;
};

//==============================================================================
template  <typename Operation, typename Arg1, typename Return = void>
struct  Bind1st
{
  Bind1st(Operation op_, Arg1 arg1_)
  : op(op_),
    arg1(arg1_)
  {}

  template <typename Arg2>
  Return  operator()(Arg2 arg2)
  {
    return op(arg1, arg2);
  }
  
  Operation op;
  Arg1      arg1;
};

//==============================================================================
template  <typename Operation, typename Arg2, typename Return = void>
struct  Bind2nd
{
  Bind2nd(Operation op_, Arg2 arg2_)
  : op(op_),
    arg2(arg2_)
  {}

  template <typename Arg1>
  Return  operator()(Arg1 arg1)
  {
    return op(arg1, arg2);
  }
  
  Operation op;
  Arg2      arg2;
};

//==============================================================================
///@brief MemberCallers can be used to forward generic callbacks to member
/// methods taking 0 - 3 arguments
template  <class Callee, void(Callee::*pMethod)()>
void  MemberCaller(void* pData)
{
  (static_cast<Callee*>(pData)->*pMethod)();
}

//==============================================================================
template  <class Callee, typename Arg1,
  void(Callee::*pMethod)(Arg1)>
void  MemberCaller(void* pData, Arg1 arg1)
{
  (static_cast<Callee*>(pData)->*pMethod)(arg1);
}

//==============================================================================
template  <class Callee, typename Arg1, typename Arg2,
  void(Callee::*pMethod)(Arg1, Arg2)>
void  MemberCaller(void* pData, Arg1 arg1, Arg2 arg2)
{
  (static_cast<Callee*>(pData)->*pMethod)(arg1, arg2);
}

//==============================================================================
template  <class Callee, typename Arg1, typename Arg2, typename Arg3,
  void(Callee::*pMethod)(Arg1, Arg2, Arg3)>
void  MemberCaller(void* pData, Arg1 arg1, Arg2 arg2, Arg3 arg3)
{
  (static_cast<Callee*>(pData)->*pMethod)(arg1, arg2, arg3);
}

}  // XR

#endif  //XR_FUNCTORS_HPP

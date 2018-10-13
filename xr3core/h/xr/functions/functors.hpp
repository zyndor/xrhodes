#ifndef XR_FUNCTORS_HPP
#define XR_FUNCTORS_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/types/typeutils.hpp"
#include <algorithm>
#include <functional>
#include <iostream>

namespace xr
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
///@brief Sends data down the std::ostream @a stream that you have specified
/// on construction.
template  <typename T>
struct  OStreamer: protected ValueFunctorCore<std::ostream*>
{
  typedef ValueFunctorCore<std::ostream*> BaseType;

  OStreamer(std::ostream* stream, char delimiter)
  : BaseType(stream),
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
  p = nullptr;
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
    m_method(pm)
  {}

  Return  operator()(Arg o)
  {
    return (BaseType::m_value.*m_method)(o);
  }

  Method  m_method;
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

  Caller(Method method, Arg a)
  : m_method(method),
    m_arg(a)
  {}

  Return  operator()(Callee& object) const
  {
    return (object.*m_method)(m_arg);
  }

  Return operator()(Callee* object) const
  {
    return (object->*m_method)(m_arg);
  }

  Method  m_method;
  Arg m_arg;
};

//==============================================================================
template  <class Callee, typename Return>
struct  Caller<Callee, Return, void>
{
  typedef typename TypeSelect<IsConst<Callee>::kResult,
    Return(Callee::*)()const,
    Return(Callee::*)()>::Type Method;

  explicit Caller(Method method)
  : m_method(method)
  {}

  Return  operator()(Callee& object) const
  {
    return (object.*m_method)();
  }

  Return operator()(Callee* object) const
  {
    return (object->*m_method)();
  }

  Method  m_method;
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

}  // xr

#endif  //XR_FUNCTORS_HPP

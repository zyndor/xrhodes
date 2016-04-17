//
// Nuclear Heart Games
// XRhodes
//
// types.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    16/07/2011
//
// copyright (c) 2011 - 2016. All rights reserved.
//
//==============================================================================
#ifndef XR_TYPESUTILS_HPP
#define XR_TYPESUTILS_HPP

namespace XR
{

//==============================================================================
///@brief Used in type operations, this type indicates that the result is not
/// meaningful and should not be used.
class Nontype
{};

//==============================================================================
///@brief	The type itself.
template  <typename T>
struct Identity 
{
  typedef T Type;
};

//==============================================================================
///@brief Ternary typedef operator. Based on the value of @a kCheck,
/// propagates the definition of @a A (if true) or @a B (if false) as @a Type.
template  <bool kCheck, typename A, typename B>
struct  TypeSelect
{
  typedef A Type;
};

template  <typename A, typename B>
struct  TypeSelect<false, A, B>
{
  typedef B Type;
};

//==============================================================================
///@brief Tells you whether T is a constant type. If it is, it defines the
/// corresponding non-const type in UnconstType.
template  <typename T>
struct  IsConst
{
  enum { kResult = false };

  typedef Nontype UnconstType;
};

template  <typename T>
struct  IsConst<const T>
{
  enum { kResult = true };

  typedef T UnconstType;
};

//==============================================================================
///@brief Tells you whether T is a pointer type. If it is, it defines the
/// corresponding pointed type in PointedType.
template  <typename T>
struct  IsPointer
{
  enum { kResult = false };

  typedef Nontype  PointedType;
};

template  <typename T>
struct  IsPointer<T*>
{
  enum { kResult = true };

  typedef T PointedType;
};

//==============================================================================
///@brief Tells you whether T0 is the same type T1.
template	<typename T0, typename T1>
struct	IsEqualType
{
	enum { kResult = false };
};

template	<typename T0>
struct	IsEqualType<T0, T0>
{
	enum { kResult = true };
};

//==============================================================================
template  <typename T>
struct  UnConst
{
  typedef T Type;
};

template  <typename T>
struct  UnConst<const T>
{
  typedef T Type;
};

//==============================================================================
template  <typename T>
struct  UnPointer
{
  typedef T Type;
};

template  <typename T>
struct  UnPointer<T*>
{
  typedef T Type;
};

//==============================================================================
///@brief Enforces T to be derived from B via implicit upcasting.
template  <class T, class B>
class AssertBase
{
  static void Constraint(T* p)
  {
    B* pBase = p;
  }
 
public:
  AssertBase()
  {
    void(*p)(T*) = Constraint;
  }
};

//==============================================================================
namespace
{
template  <typename T>
inline
size_t  TypeIdImpl()
{
  static char c;
  return reinterpret_cast<size_t>(&c);
}

}	// noname

///@brief Static typeId based on of address of function-static variable.
///@note  TypeId() doesn't persist across executions/platforms.
///@note	Discards constness and pointerness.
template	<typename T>
inline
size_t	TypeId()
{
	return TypeIdImpl<typename UnPointer<typename UnConst<T>::Type>::Type>();
}

}  // XR

#endif  //XR_TYPESUTILS_HPP

#ifndef XR_TYPESUTILS_HPP
#define XR_TYPESUTILS_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <cstddef>

namespace xr
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
namespace detail
{
template  <typename T>
struct TypeId
{
  static size_t Get()
  {
    return reinterpret_cast<size_t>(&s);
  }

private:
  static char s;
};

template <typename T>
char TypeId<T>::s; // don't actually care about the value

}	// detail

///@brief Static typeId based on the address of a static variable of a template
/// class.
///@note  TypeId() doesn't persist across executions/platforms.
///@note	Discards constness and pointerness of T.
template	<typename T>
inline
size_t	TypeId()
{
	return detail::TypeId<typename UnPointer<typename UnConst<T>::Type>::Type>::Get();
}

}  // xr

#endif  //XR_TYPESUTILS_HPP

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
#include "xr/warnings.hpp"
#include <type_traits>
#include <cstddef>

namespace xr
{

//==============================================================================
///@brief Used in type operations, this type indicates that the result is not
/// meaningful and should not be used.
class Nontype
{};

//==============================================================================
///@brief  The type itself.
template  <typename T>
struct Identity
{
  typedef T Type;
};

XR_WARNINGS_PUSH
XR_WARNINGS_IGNORE_DEPRECATION
//==============================================================================
///@brief Ternary typedef operator. Based on the value of @a kCheck,
/// propagates the definition of @a A (if true) or @a B (if false) as @a Type.
template  <bool kCheck, typename A, typename B>
struct [[deprecated("Use std::conditional<>")]] TypeSelect
{
  typedef A Type;
};

template  <typename A, typename B>
struct [[deprecated("Use std::conditional<>")]] TypeSelect<false, A, B>
{
  typedef B Type;
};

//==============================================================================
///@brief Tells you whether T is a constant type. If it is, it defines the
/// corresponding non-const type in UnconstType.
template  <typename T>
struct [[deprecated("Use std::is_const<>")]] IsConst
{
  enum { kResult = false };

  typedef Nontype UnconstType;
};

template  <typename T>
struct [[deprecated("Use std::is_const<>")]] IsConst<const T>
{
  enum { kResult = true };

  typedef T UnconstType;
};

//==============================================================================
///@brief Tells you whether T is a pointer type. If it is, it defines the
/// corresponding pointed type in PointedType.
template  <typename T>
struct [[deprecated("Use std::conditional<>")]] IsPointer
{
  enum { kResult = false };

  typedef Nontype  PointedType;
};

template  <typename T>
struct [[deprecated("Use std::conditional<>")]] IsPointer<T*>
{
  enum { kResult = true };

  typedef T PointedType;
};

//==============================================================================
///@brief Tells you whether T0 is the same type T1.
template  <typename T0, typename T1>
struct [[deprecated("Use std::is_same<>")]] IsEqualType
{
  enum { kResult = false };
};

template  <typename T0>
struct [[deprecated("Use std::is_same<>")]] IsEqualType<T0, T0>
{
  enum { kResult = true };
};

//==============================================================================
template  <typename T>
struct [[deprecated("Use std::remove_const<>")]] UnConst
{
  typedef T Type;
};

template  <typename T>
struct [[deprecated("Use std::remove_const<>")]] UnConst<const T>
{
  typedef T Type;
};

//==============================================================================
template  <typename T>
struct [[deprecated("Use std::remove_pointer<>")]] UnPointer
{
  typedef T Type;
};

template  <typename T>
struct [[deprecated("Use std::remove_pointer<>")]] UnPointer<T*>
{
  typedef T Type;
};

XR_WARNINGS_POP

//==============================================================================
namespace detail
{
template  <typename T>
struct TypeId
{
  static size_t Get() noexcept
  {
    return reinterpret_cast<size_t>(&s);
  }

private:
  static char s;
};

template <typename T>
char TypeId<T>::s; // don't actually care about the value

} // detail

///@brief Provides a static typeId based on the type of T after stripping its pointerness,
/// referenceness and cv-qualifiers.
///@note  TypeId() doesn't persist across executions/platforms.
template  <typename T>
[[nodiscard]] inline
size_t  TypeId() noexcept
{
  return detail::TypeId<
    typename std::remove_pointer<typename std::decay<T>::type>::type
  >::Get();
}

}  // xr

#endif  //XR_TYPESUTILS_HPP

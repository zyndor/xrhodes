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

//==============================================================================
///@brief Ternary typedef operator. Based on the value of @a kCheck,
/// propagates the definition of @a A (if true) or @a B (if false) as @a Type.
template  <bool kCheck, typename A, typename B>
struct TypeSelect
{
  [[deprecated("Use std::conditional<>")]]
  typedef A Type;
};

template  <typename A, typename B>
struct TypeSelect<false, A, B>
{
  [[deprecated("Use std::conditional<>")]]
  typedef B Type;
};

//==============================================================================
///@brief Tells you whether T is a constant type. If it is, it defines the
/// corresponding non-const type in UnconstType.
template  <typename T>
struct IsConst
{
  enum [[deprecated("Use std::conditional<>")]] { kResult = false };

  [[deprecated("Use std::conditional<>")]]
  typedef Nontype UnconstType;
};

template  <typename T>
struct IsConst<const T>
{
  enum [[deprecated("Use std::conditional<>")]] { kResult = true };

  [[deprecated("Use std::conditional<>")]]
  typedef T UnconstType;
};

//==============================================================================
///@brief Tells you whether T is a pointer type. If it is, it defines the
/// corresponding pointed type in PointedType.
template  <typename T>
struct IsPointer
{
  enum [[deprecated("Use std::is_pointer<>")]] { kResult = false };

  [[deprecated("Use std::is_pointer<>")]]
  typedef Nontype  PointedType;
};

template  <typename T>
struct IsPointer<T*>
{
  enum [[deprecated("Use std::is_pointer<>")]] { kResult = true };

  [[deprecated("Use std::is_pointer<>")]]
  typedef T PointedType;
};

//==============================================================================
///@brief Tells you whether T0 is the same type T1.
template  <typename T0, typename T1>
struct IsEqualType
{
  enum [[deprecated("Use std::is_same<>")]] { kResult = false };
};

template  <typename T0>
struct IsEqualType<T0, T0>
{
  enum [[deprecated("Use std::is_same<>")]] { kResult = true };
};

//==============================================================================
template  <typename T>
struct UnConst
{
  [[deprecated("Use std::remove_const<>")]]
  typedef T Type;
};

template  <typename T>
struct UnConst<const T>
{
  [[deprecated("Use std::remove_const<>")]]
  typedef T Type;
};

//==============================================================================
template  <typename T>
struct UnPointer
{
  [[deprecated("Use std::remove_pointer<>")]]
  typedef T Type;
};

template  <typename T>
struct UnPointer<T*>
{
  [[deprecated("Use std::remove_pointer<>")]]
  typedef T Type;
};

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

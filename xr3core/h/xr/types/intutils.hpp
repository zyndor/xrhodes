#ifndef XR_INTUTILS_HPP
#define XR_INTUTILS_HPP
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
#include <limits>

namespace xr
{

///@return Whether the given @a value (of integral type U) is representable using
/// integral type T.
template <typename T, typename U,
  typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value &&
    std::is_signed<T>::value == std::is_signed<U>::value>::type* = nullptr>
[[nodiscard]]
constexpr bool Representable([[maybe_unused]] U value)
{
  if constexpr (sizeof(T) >= sizeof(U))
  {
    return true;
  }
  else
  {
    return value >= static_cast<U>(std::numeric_limits<T>::min()) &&
      value <= static_cast<U>(std::numeric_limits<T>::max());
  }
}

///@return Whether the given @a value (of integral type U) is representable using
/// integral type T.
template <typename T, typename U,
  typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value &&
    !std::is_signed<T>::value && std::is_signed<U>::value>::type* = nullptr>
[[nodiscard]]
constexpr bool Representable(U value)
{
  const bool isGreaterEqual0 = value >= 0;
  if constexpr (sizeof(T) > sizeof(U))
  {
    return isGreaterEqual0;
  }
  else
  {
    return isGreaterEqual0 && value <= static_cast<U>(std::numeric_limits<T>::max());
  }
}

///@return Whether the given @a value (of integral type U) is representable using
/// integral type T.
template <typename T, typename U,
  typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value &&
    std::is_signed<T>::value && !std::is_signed<U>::value>::type* = nullptr>
[[nodiscard]]
constexpr bool Representable([[maybe_unused]] U value)
{
  if constexpr (sizeof(T) > sizeof(U))
  {
    return true;
  }
  else
  {
    return value <= static_cast<U>(std::numeric_limits<T>::max());
  }
}

}

#endif // XR_INTUTILS_HPP

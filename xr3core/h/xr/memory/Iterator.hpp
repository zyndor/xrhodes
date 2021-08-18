#ifndef XR_ITERATOR_HPP
#define XR_ITERATOR_HPP
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

namespace xr
{

//==============================================================================
///@brief Carries a non-owned pointer, which is incremented automatically, every
/// time the object is dereferenced.
template <typename T>
class Iterator
{
public:
  // structors
  explicit constexpr Iterator(T* data) noexcept;

  // general
  ///@return Pointer to the currently pointed to element.
  ///@note Not necessarily valid; know your data.
  T* Get();

  ///@return Const pointer to the currently pointed to element.
  ///@note Not necessarily valid; know your data.
  T const* Get() const;

  // operators
  ///@brief Returns a reference to the currently pointed-to element, then increments
  /// the internal pointer.
  ///@note The element must be valid.
  T& operator*();

  ///@brief Returns a pointer to the currently pointed-to element, then increments
  /// the internal pointer.
  ///@note The element must be valid.
  T* operator->();

private:
  // data
  T* mData; // no ownership
};

// deduction guide
template <typename T>
Iterator(T*) -> Iterator<std::decay_t<T>>;

//==============================================================================
// inline implementation
//==============================================================================
template <typename T>
constexpr bool operator==(Iterator<T> const& lhs, Iterator<T> const& rhs) noexcept
{
  return lhs.mData == rhs.mData;
}

//==============================================================================
template <typename T>
constexpr bool operator!=(Iterator<T> const& lhs, Iterator<T> const& rhs) noexcept
{
  return !(lhs == rhs);
}

//==============================================================================
template <typename T>
constexpr Iterator<T>::Iterator(T* data) noexcept :
  mData{ data }
{}

//==============================================================================
template <typename T>
T* Iterator<T>::Get()
{
  return mData;
}

//==============================================================================
template <typename T>
T const* Iterator<T>::Get() const
{
  return mData;
}

//==============================================================================
template <typename T>
T& Iterator<T>::operator*()
{
  return *operator->();
}

//==============================================================================
template <typename T>
T* Iterator<T>::operator->()
{
  return mData++;
}

}

#endif //XR_ITERATOR_HPP

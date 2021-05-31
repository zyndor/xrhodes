#ifndef XR_STICK_HPP
#define XR_STICK_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <cstdint>

namespace xr
{

//==============================================================================
///@brief Immutable string type which stores the size of the string, and offers
/// range based for support which ignores terminating null.
struct [[deprecated("Use std::string_view.")]] Stick
{
  // types
  using SizeType = uint32_t;

  // data
  const char* const mData;
  const SizeType mSize;

  // structors
  template <SizeType kLength>
  constexpr Stick(char const(&string)[kLength]);

  // range based for support
  const char* const begin() const;
  const char* const end() const;

  // operator overloads
    operator const char*() const;
};

//==============================================================================
// implementation
//==============================================================================
template <Stick::SizeType kLength>
constexpr Stick::Stick(char const(&string)[kLength])
: mData(string),
  mSize(kLength - 1)
{}

//==============================================================================
const char* const Stick::begin() const
{
  return mData;
}

//==============================================================================
const char* const Stick::end() const
{
  return mData + mSize;
}

//==============================================================================
Stick::operator const char*() const
{
  return mData;
}

}

#endif  //XR_STICK_HPP

#ifndef XR_RETURNSELF_HPP
#define XR_RETURNSELF_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

namespace xr
{

template <typename... Args>
class ReturnSelf;

namespace detail
{
template <typename... Args>
using ReturnSelf = xr::ReturnSelf<Args...> (*)(Args...);
}

//==============================================================================
///@brief Return type for functions required to return a pointer to a function of
/// a signature identical to theirs, as typically required for state machines.
/// This is achieved through forward declaration and implicit conversions.
///@usage ReturnSelf<Input, Etc> StateFunction(Input, Etc);
template <typename... Args>
class ReturnSelf
{
public:
  // types
  using Type = detail::ReturnSelf<Args...>;

  // structors
  ReturnSelf(Type fn)
  : mFn(fn)
  {}

  // operator overloads
    operator Type()
  {
    return mFn;
  }

private:
  // data
  Type  mFn;
};

}

#endif  //XR_RETURNSELF_HPP

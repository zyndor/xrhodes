#ifndef XR_LINKED_HPP
#define XR_LINKED_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/types/Linkable.hpp"

namespace xr
{

//==============================================================================
///@brief Linkable type whose each new instance adds itself to the end of its list
/// upon construction, and unlinks itself upon destruction.
///@par The primary intended use of Linked<> is to tie together objects that may
/// be defined in different translation units, typically as globals (file statics).
/// As such, no assumption should be made about the order of these objects in
/// the list.
template <typename T>
class Linked: private Linkable<T>
{
public:
  // using
  using Linkable<T>::ForEach;

  // structors
  Linked(T& obj);
  ~Linked() = default;
};

//==============================================================================
template<typename T>
Linked<T>::Linked(T& obj)
: Linkable<T>(obj)
{
  Linkable<T>::Link();
}

}

#endif //XR_LINKED_HPP

#ifndef XR_REVERSEADAPTER_HPP
#define XR_REVERSEADAPTER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <iterator>

namespace xr
{

//==============================================================================
///@brief Allows reverse traversal of containers and static arrays in range based
// for loops by overloading the meaning of begin() and end().
template <class C>
class ReverseAdaptor
{
public:
  ReverseAdaptor(C& c)
  : mContainer(c)
  {}

  typename C::reverse_iterator begin()
  {
    return mContainer.rbegin();
  }

  typename C::reverse_iterator end()
  {
    return mContainer.rend();
  }

private:
  C& mContainer;
};

template <typename T, size_t kN>
class ReverseAdaptor<T[kN]>
{
public:
  ReverseAdaptor(T (&array)[kN])
  : mArray(array)
  {}

  typename std::reverse_iterator<T*> begin()
  {
    return std::rbegin(mArray);
  }

  typename std::reverse_iterator<T*> end()
  {
    return std::rend(mArray);
  }

private:
  T (&mArray)[kN];
};

//==============================================================================
///@brief Makes a ReverseAdaptor given a @a container.
template <class C>
ReverseAdaptor<C> MakeReverseAdaptor(C& container)
{
  return ReverseAdaptor<C>(container);
}

//==============================================================================
///@brief Makes a ReverseAdaptor given a static @a array.
template <typename T, size_t kN>
ReverseAdaptor<T[kN]> MakeReverseAdaptor(T (&array)[kN])
{
  return ReverseAdaptor<T[kN]>(array);
}

}

#endif //XR_REVERSEADAPTER_HPP

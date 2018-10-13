#ifndef XR_ACTOR_HPP
#define XR_ACTOR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Animation.hpp"
#include "xr/Hash.hpp"
#include <map>

namespace xr
{

//==============================================================================
template <class T>
class Actor
{
public:
  // types
  typedef T               Type;
  typedef Animation<Type> AnimationType;

  typedef std::map<uint32_t, AnimationType>  MapType;

  // data
  MapType actions;

  // structors
  Actor();
  ~Actor();

  // general
  const AnimationType*  GetAction(uint32_t nameHash) const;
  const AnimationType*  GetAction(const char* name) const;
};

//==============================================================================
//  implementation
//==============================================================================
template <class T>
Actor<T>::Actor()
: actions()
{}

//==============================================================================
template <class T>
Actor<T>::~Actor()
{}

//==============================================================================
template <class T>
const typename Actor<T>::AnimationType* Actor<T>::GetAction( uint32_t nameHash ) const
{
  typename MapType::const_iterator iFind(actions.find(nameHash));
  return iFind != actions.end() ? &iFind->second : 0;
}

//==============================================================================
template <class T>
const typename Actor<T>::AnimationType* Actor<T>::GetAction( const char* name ) const
{
  XR_ASSERT(Actor, name != nullptr);
  const AnimationType*  action = GetAction(Hash::String(name));
  XR_ASSERTMSG(Actor, action != nullptr,
    ("Action '%s' could not be found in Actor %p", name, this));
  return action;
}

} // xr

#endif

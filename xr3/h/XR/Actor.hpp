//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_ACTOR_HPP
#define XR_ACTOR_HPP

#include <map>
#include "Animation.hpp"
#include "Hash.hpp"

namespace XR
{

//==============================================================================
template <class T>
class Actor
{
  XR_MANAGED_DECL(Actor<T>)

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
  const AnimationType*  GetAction(const char* pName) const;
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
const typename Actor<T>::AnimationType* Actor<T>::GetAction( const char* pName ) const
{
  XR_ASSERT(Actor, pName != 0);
  const AnimationType*  pAction(GetAction(Hash::String(pName)));
  XR_ASSERTMSG(Actor, pAction != 0,
    ("Action '%s' could not be found in Actor %p", pName, this));
  return pAction;
}

} // XR

#endif

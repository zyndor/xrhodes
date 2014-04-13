//
// Nuclear Heart Games
// XRhodes
//
// AnimationPlayer.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    23/05/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_ANIMATIONPLAYER_HPP
#define XR_ANIMATIONPLAYER_HPP

#include "Animation.hpp"

namespace XR
{

//==============================================================================
class AnimationPlayerCore
{
protected:
  // static
  static bool IsActivePredicate(const AnimationPlayerCore& player);
  static bool IsLeastActivePredicate(const AnimationPlayerCore& player,
    const AnimationPlayerCore& playerMin);

  // data
  float  tLife;

  // structors
  AnimationPlayerCore();
  ~AnimationPlayerCore();

  // general
  bool  IsActive() const;

  // data
  void*  m_pTarget; // no ownership
};

//==============================================================================
template  <class T>
class AnimationPlayer:  protected AnimationPlayerCore
{
public:
  // types
  typedef T Type;
  typedef Animation<Type> AnimationType;
  typedef typename AnimationType::Instance  AnimationInstance;
  
  // using
  using AnimationPlayerCore::tLife;
  using AnimationPlayerCore::IsActivePredicate;
  using AnimationPlayerCore::IsLeastActivePredicate;
  using AnimationPlayerCore::IsActive;

  // data
  AnimationInstance anim;
  
  // structors
  AnimationPlayer();
  ~AnimationPlayer();
    
  // general
  void  SetTarget(Type& target);
  
  void  Start(const AnimationType* pAnim, float timeScale, float lifespan,
    Type& pTarget);
    
  void  Update(float tDelta);
};

//==============================================================================
/*
  implementation
*/
//==============================================================================
inline
bool  AnimationPlayerCore::IsActive() const
{
  return tLife > .0f;
}

//==============================================================================
template  <class T>
AnimationPlayer<T>::AnimationPlayer()
: AnimationPlayerCore(),
  anim()
{}
  
//==============================================================================
template  <class T>
AnimationPlayer<T>::~AnimationPlayer()
{}
  
//==============================================================================
template  <class T>
void  AnimationPlayer<T>::SetTarget(Type& target)
{
  m_pTarget = &target;
  target = anim.GetCurrentFrameData();
}
  
//==============================================================================
template  <class T>
void  AnimationPlayer<T>::Start(const AnimationType* pAnim, float timeScale,
  float lifespan, T& pTarget)
{
  XR_ASSERT(AnimationPlayer, pAnim != 0);
  SetTarget(pTarget);
  anim.SetAnimation(pAnim, timeScale);
}
  
//==============================================================================
template  <class T>
void  AnimationPlayer<T>::Update(float tDelta)
{
  XR_ASSERT(AnimationPlayer, tLife > .0f);
  if(tLife > .0f)
  {
    tLife -= tDelta;
    anim.Update(tDelta);
    *static_cast<Type*>(m_pTarget) = anim.GetCurrentFrameData();
  }
}

} // XR

#endif // XR_ANIMATIONPLAYER_HPP
#ifndef XR_ANIMATIONPLAYER_HPP
#define XR_ANIMATIONPLAYER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Animation.hpp"

namespace xr
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

  // general
  bool  IsActive() const;

  // data
  void*  m_target; // no ownership
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

  // general
  void  SetTarget(Type& target);

  void  Start(const AnimationType* anim, float timeScale, float lifespan,
    Type& target);

  void  Update(float tDelta);
};

//==============================================================================
// implementation
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
void  AnimationPlayer<T>::SetTarget(Type& target)
{
  m_target = &target;
  target = anim.GetCurrentFrameData();
}

//==============================================================================
template  <class T>
void  AnimationPlayer<T>::Start(const AnimationType* anim_, float timeScale,
  float lifespan, T& target)
{
  XR_ASSERT(AnimationPlayer, anim_ != nullptr);
  SetTarget(target);
  anim.SetAnimation(anim_, timeScale);
}

//==============================================================================
template  <class T>
void  AnimationPlayer<T>::Update(float tDelta)
{
  XR_ASSERT(AnimationPlayer, tLife > .0f);
  if (tLife > .0f)
  {
    tLife -= tDelta;
    anim.Update(tDelta);
    *static_cast<Type*>(m_target) = anim.GetCurrentFrameData();
  }
}

} // xr

#endif // XR_ANIMATIONPLAYER_HPP

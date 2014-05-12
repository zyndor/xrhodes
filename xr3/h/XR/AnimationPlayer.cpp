//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include "AnimationPlayer.hpp"

namespace XR
{

//==============================================================================
bool AnimationPlayerCore::IsActivePredicate(const AnimationPlayerCore& player)
{
  return player.IsActive();
}

//==============================================================================
bool AnimationPlayerCore::IsLeastActivePredicate(const AnimationPlayerCore& player,
  const AnimationPlayerCore& playerMin)
{
  return player.tLife < playerMin.tLife;
}

//==============================================================================
AnimationPlayerCore::AnimationPlayerCore()
: tLife(.0f),
  m_pTarget(0)
{}

//==============================================================================
AnimationPlayerCore::~AnimationPlayerCore()
{}


} // XR

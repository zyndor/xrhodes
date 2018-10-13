//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/AnimationPlayer.hpp"

namespace xr
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
  m_target(nullptr)
{}


} // xr

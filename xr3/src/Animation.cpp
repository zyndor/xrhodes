//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Animation.hpp"

namespace xr
{

//==============================================================================
AnimationCore::AnimationCore()
: frameDelay(.0f),
  m_loopFromFrame(NO_LOOP_FRAME)
{}

//==============================================================================
AnimationCore::~AnimationCore()
{}

//==============================================================================
void  AnimationCore::SetLoopFrame(int lf)
{
  m_loopFromFrame = lf;
}


} // xr

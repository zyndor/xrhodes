//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
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

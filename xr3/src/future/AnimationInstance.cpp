//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/AnimationInstance.hpp"

namespace xr
{

//==============================================================================
AnimationInstanceCore::AnimationInstanceCore()
:	timeScale(1.0),
  onFinishedData(nullptr),
  m_tFrame(.0f),
  m_frameId(0),
  m_numLoops(0)
{}

//==============================================================================
AnimationInstanceCore::~AnimationInstanceCore()
{}

//==============================================================================
void AnimationInstanceCore::ResetAnimation( float timeScale_ )
{
  timeScale = timeScale_;
  m_tFrame = .0f;
  m_frameId = 0;
  m_numLoops = 0;
}

} // xr

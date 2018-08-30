//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/ActorInstance.hpp"
#include "xr/Hash.hpp"

namespace xr
{

//==============================================================================
ActorInstanceCore::ActorInstanceCore()
: onActionFinishedData(nullptr),
  m_nameHashNextAction(0),
  m_timeScaleNextAction(1.0f)
{}

//==============================================================================
ActorInstanceCore::~ActorInstanceCore()
{}

//==============================================================================
void ActorInstanceCore::Enqueue(const char* pActionName, float timeScale)
{
  XR_ASSERT(ActorInstanceCore, pActionName != 0);
  Enqueue(Hash::String32(pActionName), timeScale);
}

//==============================================================================
void ActorInstanceCore::Enqueue(uint32_t nameHashAction, float timeScale)
{
  m_nameHashNextAction = nameHashAction;
  m_timeScaleNextAction = timeScale;
}

} // xr

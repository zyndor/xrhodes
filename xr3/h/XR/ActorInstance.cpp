//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "ActorInstance.hpp"
#include <XR/Hash.hpp>

namespace XR
{

//==============================================================================
ActorInstanceCore::ActorInstanceCore()
: pActionFinishedCbData(0),
  m_nameHashNextAction(0),
  m_timeScaleNextAction(1.0f)
{}

//==============================================================================
ActorInstanceCore::~ActorInstanceCore()
{}

//==============================================================================
void ActorInstanceCore::Enqueue( const char* pActionName, float timeScale )
{
  XR_ASSERT(ActorInstanceCore, pActionName != 0);
  Enqueue(Hash::String(pActionName), timeScale);
}

//==============================================================================
void ActorInstanceCore::Enqueue( uint32_t nameHashAction, float timeScale )
{
  m_nameHashNextAction = nameHashAction;
  m_timeScaleNextAction = timeScale;
}

} // XR

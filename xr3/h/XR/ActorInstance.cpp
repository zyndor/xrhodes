#include "ActorInstance.hpp"
#include "Hash.hpp"

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
void ActorInstanceCore::Enqueue( uint32 nameHashAction, float timeScale )
{
  m_nameHashNextAction = nameHashAction;
  m_timeScaleNextAction = timeScale;
}

} // XR

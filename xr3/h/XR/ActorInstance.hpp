#ifndef XR_ACTORINSTANCE_HPP
#define XR_ACTORINSTANCE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "Actor.hpp"
#include "AnimationInstance.hpp"

namespace XR
{

//==============================================================================
class ActorInstanceCore
{
public:
  // data
  void* onActionFinishedData;

  // structors
  ActorInstanceCore();
  ~ActorInstanceCore();

  // general
  void  Enqueue(const char* actionName, float timeScale);
  void  Enqueue(uint32_t nameHashAction, float timeScale);

protected:
  // data
  uint32_t  m_nameHashNextAction;
  float     m_timeScaleNextAction;
};

//==============================================================================
template  <class T>
  class ActorInstance:  protected ActorInstanceCore
{
public:
  // typedef
  typedef T Type;

  typedef Actor<Type>             ActorType;
  typedef AnimationInstance<Type> AnimationInstanceType;

  typedef void(*ActionFinishedCallback)(ActorInstance& inst, void* data);

  // using
  using ActorInstanceCore::Enqueue;

  // data
  ActionFinishedCallback  onActionFinished;

  // structors
  ActorInstance();
  ~ActorInstance();

  // general
  void  SetActor(const ActorType* actor);

  const AnimationInstanceType&  GetAnimation() const;

  void  Request(const char* actionName, float timeScale);
  void  Request(uint32_t nameHashAction, float timeScale);

  void  RequestChange(const char* actionName, float timeScale);
  void  RequestChange(uint32_t nameHashAction, float timeScale);

  void  Update(float tDelta);

protected:
  // static
  static void AnimationFinishedCallback(AnimationInstanceType& inst,
    void* data);

  // data
  const ActorType*      m_actor;
  AnimationInstanceType m_anim;
};

//==============================================================================
// implementation
//==============================================================================
template  <class T>
void ActorInstance<T>::AnimationFinishedCallback( AnimationInstanceType& inst,
  void* data )
{
  ActorInstance<T>* instance = static_cast<ActorInstance<T>*>(data);
  if (instance->m_nameHashNextAction != 0)
  {
    if (instance->onActionFinished != 0)
    {
      (*instance->onActionFinished)(*instance, instance->onActionFinishedData);
    }

    instance->Request(instance->m_nameHashNextAction, instance->m_timeScaleNextAction);
    instance->m_nameHashNextAction = 0;
  }
}

//==============================================================================
template  <class T>
ActorInstance<T>::ActorInstance()
: ActorInstanceCore(),
  onActionFinished(nullptr),
  m_actor(nullptr),
  m_anim()
{
  m_anim.onFinished = AnimationFinishedCallback;
  m_anim.onFinishedData = this;
}

//==============================================================================
template  <class T>
ActorInstance<T>::~ActorInstance()
{}

//==============================================================================
template  <class T>
void ActorInstance<T>::SetActor( const ActorType* actor )
{
  m_actor = actor;
  m_anim.ResetAnimation(1.0f);
}

//==============================================================================
template  <class T>
const AnimationInstance<T>& ActorInstance<T>::GetAnimation() const
{
  return m_anim;
}

//==============================================================================
template  <class T>
void ActorInstance<T>::Request( const char* actionName, float timeScale )
{
  XR_ASSERT(ActorInstance, actionName != nullptr);
  Request(Hash::String(actionName), timeScale);
}

//==============================================================================
template  <class T>
void ActorInstance<T>::Request( uint32_t nameHashAction, float timeScale )
{
  XR_ASSERT(ActorInstance, m_actor != nullptr);
  m_anim.SetAnimation(m_actor->GetAction(nameHashAction), timeScale);
}

//==============================================================================
template  <class T>
void ActorInstance<T>::RequestChange( const char* actionName, float timeScale )
{
  XR_ASSERT(ActorInstance, actionName != nullptr);
  RequestChange(Hash::String(actionName), timeScale);
}

//==============================================================================
template  <class T>
void ActorInstance<T>::RequestChange( uint32_t nameHashAction, float timeScale )
{
  XR_ASSERT(ActorInstance, m_actor != nullptr);
  const Animation<T>*  action(m_actor->GetAction(nameHashAction));
  if (action != m_anim.GetAnimation())
  {
    m_anim.SetAnimation(action, timeScale);
  }
  else
  {
    m_anim.timeScale = timeScale;
  }
}

//==============================================================================
template  <class T>
void ActorInstance<T>::Update( float tDelta )
{
  m_anim.Update(tDelta);
}

} // XR

#endif  // XR_ACTORINSTANCE_HPP

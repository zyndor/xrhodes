//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_ACTORINSTANCE_HPP
#define XR_ACTORINSTANCE_HPP

#include "Actor.hpp"
#include "AnimationInstance.hpp"

namespace XR
{

//==============================================================================
class ActorInstanceCore
{
public:
  // data
  void* pActionFinishedCbData;
  
  // structors
  ActorInstanceCore();
  ~ActorInstanceCore();
  
  // general
  void  Enqueue(const char* pActionName, float timeScale);
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
  
  typedef void(*ActionFinishedCallback)(ActorInstance& inst, void* pData);

  // using
  using ActorInstanceCore::Enqueue;

  // data
  ActionFinishedCallback  pActionFinishedCb;
  
  // structors
  ActorInstance();
  ~ActorInstance();
  
  // general
  void  SetActor(const ActorType* pActor);
  
  const AnimationInstanceType&  GetAnimation() const;
  
  void  Request(const char* pActionName, float timeScale);
  void  Request(uint32_t nameHashAction, float timeScale);
  
  void  RequestChange(const char* pActionName, float timeScale);
  void  RequestChange(uint32_t nameHashAction, float timeScale);

  void  Update(float tDelta);
  
protected:
  // static
  static void AnimationFinishedCallback(AnimationInstanceType& inst,
    void* pData);
  
  // data
  const ActorType*      m_pActor;
  AnimationInstanceType m_anim;
};

//==============================================================================
// implementation
//==============================================================================
template  <class T>
void ActorInstance<T>::AnimationFinishedCallback( AnimationInstanceType& inst,
  void* pData )
{
  ActorInstance<T>* pInst(static_cast<ActorInstance<T>*>(pData));
  if (pInst->m_nameHashNextAction != 0)
  {
    if (pInst->pActionFinishedCb != 0)
    {
      (*pInst->pActionFinishedCb)(*pInst, pInst->pActionFinishedCbData);
    }

    pInst->Request(pInst->m_nameHashNextAction, pInst->m_timeScaleNextAction);
    pInst->m_nameHashNextAction = 0;
  }
}

//==============================================================================
template  <class T>
ActorInstance<T>::ActorInstance()
: ActorInstanceCore(),
  pActionFinishedCb(0),
  m_pActor(0),
  m_anim()
{
  m_anim.pFinishedCb = AnimationFinishedCallback;
  m_anim.pFinishedCbData = this;
}

//==============================================================================
template  <class T>
ActorInstance<T>::~ActorInstance()
{}

//==============================================================================
template  <class T>
void ActorInstance<T>::SetActor( const ActorType* pActor )
{
  m_pActor = pActor;
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
void ActorInstance<T>::Request( const char* pActionName, float timeScale )
{
  XR_ASSERT(ActorInstance, pActionName != 0);
  Request(Hash::String(pActionName), timeScale);
}

//==============================================================================
template  <class T>
void ActorInstance<T>::Request( uint32_t nameHashAction, float timeScale )
{
  XR_ASSERT(ActorInstance, m_pActor != 0);
  m_anim.SetAnimation(m_pActor->GetAction(nameHashAction), timeScale);
}

//==============================================================================
template  <class T>
void ActorInstance<T>::RequestChange( const char* pActionName, float timeScale )
{
  XR_ASSERT(ActorInstance, pActionName != 0);
  RequestChange(Hash::String(pActionName), timeScale);
}

//==============================================================================
template  <class T>
void ActorInstance<T>::RequestChange( uint32_t nameHashAction, float timeScale )
{
  XR_ASSERT(ActorInstance, m_pActor != 0);
  const Animation<T>*  pAction(m_pActor->GetAction(nameHashAction));
  if (pAction != m_anim.GetAnimation())
  {
    m_anim.SetAnimation(pAction, timeScale);
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

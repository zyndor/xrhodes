#ifndef XR_ANIMATIONINSTANCE_HPP
#define XR_ANIMATIONINSTANCE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "Animation.hpp"

namespace xr
{

//==============================================================================
class AnimationInstanceCore
{
public:
  // data
  float timeScale;

  void* onFinishedData;

  // structors
  AnimationInstanceCore();
  ~AnimationInstanceCore();

  // general
  float	GetFrameTime() const;
  int   GetFrameId() const;
  int   GetNumLoops() const;
  void  ResetAnimation(float timeScale);

protected:
  // data
  float	m_tFrame;
  int		m_frameId;
  int   m_numLoops;
};

//==============================================================================
template <class T>
class	AnimationInstance:  protected AnimationInstanceCore
{
public:
  // types
  typedef T               Type;
  typedef Animation<Type> AnimationType;

  typedef void(*FinishedCallback)(AnimationInstance<T>& inst, void* data);

  // using
  using AnimationInstanceCore::timeScale;
  using AnimationInstanceCore::onFinishedData;

  using AnimationInstanceCore::GetFrameId;
  using AnimationInstanceCore::GetFrameTime;
  using AnimationInstanceCore::GetNumLoops;
  using AnimationInstanceCore::ResetAnimation;

  // data
  FinishedCallback  onFinished;

  // structors
  AnimationInstance();
  ~AnimationInstance();

  // general
  const AnimationType*  GetAnimation() const;

  void	SetAnimation(const AnimationType* anim, float timeScale);

  void	Update(float tDelta);

  bool  IsFinished() const;

  const T&	GetCurrentFrameData() const;

protected:
  // data
  const AnimationType*	m_animation;
};

//==============================================================================
// implementation
//==============================================================================
inline
float AnimationInstanceCore::GetFrameTime() const
{
  return m_tFrame;
}

//==============================================================================
inline
int AnimationInstanceCore::GetFrameId() const
{
  return m_frameId;
}

//==============================================================================
inline
int AnimationInstanceCore::GetNumLoops() const
{
  return m_numLoops;
}

//==============================================================================
template <class T>
AnimationInstance<T>::AnimationInstance()
:	AnimationInstanceCore(),
  onFinished(nullptr),
  m_animation(nullptr)
{}

//==============================================================================
template <class T>
AnimationInstance<T>::~AnimationInstance()
{}

//==============================================================================
template <class T>
const Animation<T>* AnimationInstance<T>::GetAnimation() const
{
  return m_animation;
}

//==============================================================================
template <class T>
void AnimationInstance<T>::SetAnimation( const AnimationType* anim, float timeScale )
{
  m_animation = anim;
  ResetAnimation(timeScale);
}

//==============================================================================
template <class T>
void AnimationInstance<T>::Update( float tDelta )
{
  XR_ASSERT(AnimationInstance, tDelta > .0f);
  if (m_numLoops < 1 || m_animation->IsLooping())
  {
    XR_ASSERT(AnimationInstance, timeScale >= .0f);
    m_tFrame -= tDelta * timeScale;

    int	frameSkip = 0;
    while (m_tFrame <= .0f)
    {
      ++frameSkip;
      m_tFrame += m_animation->frameDelay;
    }

    if (frameSkip > 0)
    {
      m_frameId += frameSkip;
      if (m_frameId >= m_animation->frames.size())
      {
        m_frameId = m_animation->IsLooping() ?
          m_animation->GetLoopFrame() + (m_frameId %
            (m_animation->frames.size() - m_animation->GetLoopFrame())) :
          m_animation->frames.size() - 1;

        ++m_numLoops;
        if (onFinished)
        {
          (*onFinished)(*this, onFinishedData);
        }
      }
    }
  }
}

//==============================================================================
template <class T>
bool AnimationInstance<T>::IsFinished() const
{
  XR_ASSERT(AnimationInstance, m_animation != nullptr);
  return m_numLoops > 0 && !m_animation->IsLooping();
}

//==============================================================================
template <class T>
const T& AnimationInstance<T>::GetCurrentFrameData() const
{
  XR_ASSERT(AnimationInstance, m_animation != nullptr);
  XR_ASSERT(AnimationInstance, m_frameId >= 0);
  XR_ASSERT(AnimationInstance, static_cast<size_t>(m_frameId) <
    m_animation->frames.size());
  return m_animation->frames[m_frameId];
}

} // xr

#endif  // XR_ANIMATIONINSTANCE_HPP

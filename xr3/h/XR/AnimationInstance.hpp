//
// Nuclear Heart Games
// XRhodes
//
// AnimationInstance.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    23/05/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_ANIMATIONINSTANCE_HPP
#define XR_ANIMATIONINSTANCE_HPP

#include  "Animation.hpp"

namespace XR
{

//==============================================================================
class AnimationInstanceCore
{
public:
  // data
  float timeScale;
  
  void* pFinishedCbData;
  
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
  
  typedef void(*FinishedCallback)(AnimationInstance<T>& inst, void* pData);
  
  // using
  using AnimationInstanceCore::timeScale;
  using AnimationInstanceCore::pFinishedCbData;
  
  using AnimationInstanceCore::GetFrameId;
  using AnimationInstanceCore::GetFrameTime;
  using AnimationInstanceCore::GetNumLoops;
  using AnimationInstanceCore::ResetAnimation;
  
  // data
  FinishedCallback  pFinishedCb;

  // structors
  AnimationInstance();
  ~AnimationInstance();
  
  // general
  const AnimationType*  GetAnimation() const;
  
  void	SetAnimation(const AnimationType* pAnim, float timeScale);
  
  void	Update(float tDelta);

  bool  IsFinished() const;
  
  const T&	GetCurrentFrameData() const;
  
protected:
  // data
  const AnimationType*	m_pAnimation;
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
  pFinishedCb(0),
  m_pAnimation(0)
{}

//==============================================================================
template <class T>
AnimationInstance<T>::~AnimationInstance()
{}

//==============================================================================
template <class T>
const Animation<T>* AnimationInstance<T>::GetAnimation() const
{
  return m_pAnimation;
}

//==============================================================================
template <class T>
void AnimationInstance<T>::SetAnimation( const AnimationType* pAnim, float timeScale )
{
  m_pAnimation = pAnim;
  ResetAnimation(timeScale);
}

//==============================================================================
template <class T>
void AnimationInstance<T>::Update( float tDelta )
{
  XR_ASSERT(AnimationInstance, tDelta > .0f);
  if(m_numLoops < 1 || m_pAnimation->IsLooping())
  {
    XR_ASSERT(AnimationInstance, timeScale >= .0f);
    m_tFrame -= tDelta * timeScale;

    int	frameSkip(0);
    while(m_tFrame <= .0f)
    {
      ++frameSkip;
      m_tFrame += m_pAnimation->frameDelay;
    }

    if(frameSkip > 0)
    {
      m_frameId += frameSkip;
      if(m_frameId >= m_pAnimation->frames.size())
      {
        m_frameId = m_pAnimation->IsLooping() ?
          m_pAnimation->GetLoopFrame() + (m_frameId %
            (m_pAnimation->frames.size() - m_pAnimation->GetLoopFrame())) :
          m_pAnimation->frames.size() - 1;

        ++m_numLoops;
        if(pFinishedCb != 0)
        {
          (*pFinishedCb)(*this, pFinishedCbData);
        }
      }
    }
  }
}

//==============================================================================
template <class T>
bool AnimationInstance<T>::IsFinished() const
{
  XR_ASSERT(AnimationInstance, m_pAnimation != 0);
  return m_numLoops > 0 && !m_pAnimation->IsLooping();
}

//==============================================================================
template <class T>
const T& AnimationInstance<T>::GetCurrentFrameData() const
{
  XR_ASSERT(AnimationInstance, m_pAnimation != 0);
  XR_ASSERT(AnimationInstance, m_frameId >= 0);
  XR_ASSERT(AnimationInstance, static_cast<size_t>(m_frameId) <
    m_pAnimation->frames.size());
  return m_pAnimation->frames[m_frameId];
}

} // XR

#endif  // XR_ANIMATIONINSTANCE_HPP

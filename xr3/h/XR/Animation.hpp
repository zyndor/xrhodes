//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#if !defined	XR_ANIMATION_HPP
#define XR_ANIMATION_HPP

#include <vector>
#include "ResManager.hpp"

namespace XR
{

//==============================================================================
class AnimationCore
{
public:
  // typedef
  enum { NO_LOOP_FRAME = -1 };
  
  // data
	float frameDelay;

  // structors
  AnimationCore();
  ~AnimationCore();
  
  // general
  int   GetLoopFrame() const;
  void  SetLoopFrame(int lf);
  
protected:
  // data
  int m_loopFromFrame;
};

template	<class T>
class	Animation:  protected AnimationCore
{
  XR_MANAGED_DECL(Animation<T>)

public:
	// types
	typedef T	Type;
	
	typedef std::vector<T>	FrameVector;

  // using
  using AnimationCore::frameDelay;
  using AnimationCore::GetLoopFrame;
  using AnimationCore::SetLoopFrame;
	
	// data
	FrameVector	frames;
	
	// structors
	Animation();		
	~Animation();
  
  // general
  bool  IsLooping() const;
  float GetDuration() const;
};

//==============================================================================
// implementation
//==============================================================================
inline
int AnimationCore::GetLoopFrame() const
{
  return m_loopFromFrame;
}
  
//==============================================================================
template	<class T>
Animation<T>::Animation()
:	AnimationCore(),
  frames()
{}

//==============================================================================
template	<class T>
Animation<T>::~Animation()
{}

//==============================================================================
template  <class T>
bool  Animation<T>::IsLooping() const
{
  return static_cast<size_t>(m_loopFromFrame) < frames.size();
}

//==============================================================================
template  <class T>
float Animation<T>::GetDuration() const
{
  XR_ASSERT(Animation, frameDelay > .0f);
  return frames.size() * frameDelay;
}

} // XR

#endif
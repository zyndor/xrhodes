#include "Animation.hpp"

namespace XR
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
  

} // XR

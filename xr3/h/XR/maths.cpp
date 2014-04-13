//
// Nuclear Heart Games
// XRhodes
//
// math.cpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date  04/07/2011
//
//==============================================================================
#include "maths.hpp"

namespace XR
{

//==============================================================================
const float kRecByteMax = 1.0f / 0xFF;

const float kPi = float(M_PI);

const float kPiHalf = kPi * .5f;

const float kPi2 = kPi * 2.0f;

const float kPiHalfSqr = kPiHalf * kPiHalf;

const float kDegreesToRadians = kPi / 180.0f;

const float kRadiansToDegrees = 1.0f / kDegreesToRadians;

const float  kRecRandMax = 1.0f / (float)(RAND_MAX);

//==============================================================================
float CalcSpeedPerFrame(float v, float fps)
{
  //XR_ASSERT(CalcSpeedPerFrame, fps > .0f);
  return v / fps;
}

//==============================================================================
float CalcAccelerationPerFrame(float v, float fps)
{
  //XR_ASSERT(CalcSpeedPerFrame, fps > .0f);
  return v / (fps * fps);
}

//==============================================================================
float CalcScalingPerFrame(float v, float fps)
{
  //XR_ASSERT(CalcSpeedPerFrame, fps > .0f);
  return powf(v, 1.0f / fps);
}

}  // XR

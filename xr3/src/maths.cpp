//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/maths.hpp"

namespace xr
{

//==============================================================================
const float kRecByteMax = 1.0f / 0xFF;

const float kPi = float(M_PI);

const float kPiHalf = kPi * .5f;

const float kPi2 = kPi * 2.0f;

const float kPiHalfSqr = kPiHalf * kPiHalf;

const float kDegreesToRadians = kPi / 180.0f;

const float kRadiansToDegrees = 1.0f / kDegreesToRadians;

const float kEpsilon = .00001f;

const float  kRecRandMax = 1.0f / float(RAND_MAX);

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

//==============================================================================
bool  SolveQuadratic(float a, float b, float c, float& x0, float& x1)
{
  bool  valid = a != .0f;
  if (valid)
  {
    float root = b * b - 4.0f * a * c;
    valid = root >= .0f;
    if (valid)
    {
      root = sqrtf(root);
      a = 1.0f / (2.0f * a);
      b = -b;

      x0 = (b + root) * a;
      x1 = (b - root) * a;

      if (x0 > x1)
      {
        std::swap(x0, x1);
      }
    }
  }
  return valid;
}

}  // xr

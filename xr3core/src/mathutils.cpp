//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/math/mathutils.hpp"

namespace xr
{

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

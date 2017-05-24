//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <limits>
#include <XR/AABB.hpp>
#include <XR/maths.hpp>

namespace XR
{

//==============================================================================
const char* const AABB::karTag[] =
{
  "left",
  "bottom",
  "right",
  "top"
};

//==============================================================================
void  AABB::Import(float x, float y, float hw, float hh)
{
  left = x - hw;
  right = x + hw;
  top = y - hh;
  bottom = y + hh;
}

//==============================================================================
void  AABB::Export(float& x, float& y, float& hw, float& hh) const 
{
  x = (left + right) / 2;
  y = (top + bottom) / 2;

  hw = (right - left) / 2;
  hh = (bottom - top) / 2;
}

//==============================================================================
bool  AABB::HitTest(float x0, float y0, float dx, float dy) const
{
  float p[4] =
  {
    -dx,
    dx,
    -dy,
    dy,
  };

  float q[4] =
  {
    x0 - left,
    right - x0,
    y0 - top,
    bottom - y0,
  };

  float t0(-std::numeric_limits<float>::max());	// lowest() [C++11]
  float t1(std::numeric_limits<float>::max());
  for (int i = 0; i < 4; ++i)
  {
    if (p[i] * p[i] > .0f)
    {
      float t = q[i] / p[i];
      if (p[i] < .0f && t > t0)
      {
        t0 = t;
      }
      else if (p[i] > .0f && t < t1)
      {
        t1 = t;
      }
    }
    else
    {
      if (q[i] < .0f)
      {
        return false;
      }
    }
  }

  return t0 < t1 && t1 > .0f && t0 < 1.0f;
}

//==============================================================================
bool  AABB::HitTest(float x0, float y0, float dx, float dy, float& t) const
{
  float p[4] =
  {
    -dx,
    dx,
    -dy,
    dy,
  };

  float q[4] =
  {
    x0 - left,
    right - x0,
    y0 - top,
    bottom - y0,
  };

  float t0(-std::numeric_limits<float>::max());	// lowest() [C++11]
  float t1(std::numeric_limits<float>::max());
  for (int i = 0; i < 4; ++i)
  {
    if (p[i] * p[i] > .0f)
    {
      float tt = q[i] / p[i];
      if (p[i] < .0f && tt > t0)
      {
        t0 = tt;
      }
      else if (p[i] > .0f && tt < t1)
      {
        t1 = tt;
      }
    }
    else
    {
      if (q[i] < .0f)
      {
        return false;
      }
    }
  }

  if (t0 < t1 && t1 > .0f && t0 < 1.0f)
  {
    t = t0;
    return true;
  }
  else
  {
    return false;
  }
}

}  // XR

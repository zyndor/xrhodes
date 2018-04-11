//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Ray.hpp"
#include "XR/maths.hpp"
#include <limits>

namespace XR
{

//==============================================================================
Ray::Ray()
: position(),
  direction(),
  length(.0f)
{}

//==============================================================================
Ray::Ray(const Vector3& pos, const Vector3& dir, float l)
: position(pos),
  direction(dir),
  length(l)
{}

//==============================================================================
void  Ray::SetDirection(const Vector3& dir)
{
  float d(dir.Dot());
  XR_ASSERT(Ray, d > .0f);
  SetDirectionNormalised(dir / sqrtf(d));
}

//==============================================================================
void  Ray::SetDirectionNormalised(const Vector3& dir)
{
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  XR_ASSERTMSG(Ray, fabsf(direction.Dot() - 1.0f) < kEpsilon,
    ("Ray direction is not normalised."));
#endif
  direction = dir;
}

//==============================================================================
bool  Ray::IntersectSphere(const Vector3& pos, float radiusSqr, float& t0,
        float& t1) const
{
  Vector3 rel(position - pos);
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  XR_ASSERTMSG(Ray, fabsf(direction.Dot() - 1.0f) < kEpsilon,
    ("Ray direction is not normalised."));
#endif
  float a = direction.Dot();
  float b = 2.0f * direction.Dot(rel);
  float c = rel.Dot() - radiusSqr;
  bool  result = SolveQuadratic(a, b, c, t0, t1) && t1 > .0f && t0 < length;
  return result;
}

//==============================================================================
bool  Ray::IntersectPlane(const Vector3& pos, const Vector3& normal, float& t)
        const
{
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  XR_ASSERTMSG(Ray, fabsf(direction.Dot() - 1.0f) < kEpsilon,
    ("Ray direction is not normalised."));
#endif
  float d = normal.Dot(direction);
  bool  result = d < .0f;
  if (result)
  {
    Vector3 rel(pos - position);
    t = rel.Dot(normal) / d;
    result = t >= .0f;
  }
  return result;
}

} // XR

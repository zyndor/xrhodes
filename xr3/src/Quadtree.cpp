//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Quadtree.hpp"
#include "xr/functions/functors.hpp"

namespace xr
{

//==============================================================================
int QuadtreeCore::sMaxSubdivisions = kDefaultSubdivisionLimit;

//==============================================================================
float QuadtreeCore::CalculateMin(float hw, float hh, uint32_t maxSubdivisions)
{
  return float(static_cast<int>(floorf(hw + hh)) >> (maxSubdivisions + 1));
}

//==============================================================================
float QuadtreeCore::CalculateMin(float hw, float hh)
{
  return CalculateMin(hw, hh, sMaxSubdivisions);
}

//==============================================================================
int QuadtreeCore::GetMaxSubdivisions()
{
  return sMaxSubdivisions;
}

//==============================================================================
void QuadtreeCore::SetMaxSubdivisions(int maxSubdivisions)
{
  XR_ASSERT(QuadtreeCore, maxSubdivisions >= 0);
  sMaxSubdivisions = maxSubdivisions;
}

//==============================================================================
AABB const& QuadtreeCore::GetCandidateBox()
{
  static AABB dummy{ 0.f, 0.f, 0.f, 0.f };
  return dummy;
}

//==============================================================================
QuadtreeCore::QuadtreeCore()
: RectObject()
{}

//==============================================================================
QuadtreeCore::QuadtreeCore(Vector2 const& pos, float hw, float hh)
: RectObject(hw, hh, pos)
{}

//==============================================================================
QuadtreeCore::~QuadtreeCore()
{}

} // xr

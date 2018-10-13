//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Quadtree.hpp"
#include "xr/functors.hpp"

namespace xr
{

//==============================================================================
int       QuadtreeCore::s_maxSubdivisions = 5;
AABB      QuadtreeCore::s_hitBox;
void*     QuadtreeCore::s_callbackData = nullptr;

QuadtreeCore::UnaryCallback   QuadtreeCore::s_unaryCallback = nullptr;
QuadtreeCore::BinaryCallback  QuadtreeCore::s_binaryCallback = nullptr;

//==============================================================================
void QuadtreeCore::SetMaxSubdivisions(int maxSubdivisions)
{
  XR_ASSERT(QuadtreeCore, maxSubdivisions >= 0);
  s_maxSubdivisions = maxSubdivisions;
}

//==============================================================================
QuadtreeCore::QuadtreeCore()
: RectObject(),
  m_canSplit(false)
{}

//==============================================================================
QuadtreeCore::QuadtreeCore(const Vector2 &pos, float hw, float hh)
: RectObject(hw, hh, pos),
  m_canSplit(false)
{}

//==============================================================================
QuadtreeCore::~QuadtreeCore()
{}

} // xr

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Quadtree.hpp"
#include "XR/functors.hpp"

namespace XR
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

} // XR

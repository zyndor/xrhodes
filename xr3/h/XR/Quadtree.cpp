#include "Quadtree.hpp"
#include "functors.hpp"

namespace XR
{

//==============================================================================
int       QuadtreeCore::s_maxSubdivisions(5);
AABB      QuadtreeCore::s_candidateBox;
void*     QuadtreeCore::s_pCandidate(0);

QuadtreeCore::UnaryCallback   QuadtreeCore::s_pUnaryCallback(0);
QuadtreeCore::BinaryCallback  QuadtreeCore::s_pBinaryCallback(0);

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

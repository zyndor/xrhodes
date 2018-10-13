//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/RectObject.hpp"
#include "xr/AABB.hpp"

namespace xr
{

//==============================================================================
RectObject::RectObject(const Vector2& pos)
: m_position(pos),
  m_halfWidth(0),
  m_halfHeight(0)
{}

//==============================================================================
RectObject::RectObject(float hw, float hh, const Vector2& pos)
: m_position(pos),
  m_halfWidth(hw),
  m_halfHeight(hh)
{}

//==============================================================================
RectObject::~RectObject()
{}

//==============================================================================
void  RectObject::SetHalfWidth(float hw)
{
  m_halfWidth = hw;
}

//==============================================================================
void  RectObject::SetHalfHeight(float hh)
{
  m_halfHeight = hh;
}

//==============================================================================
void  RectObject::Export(AABB& box) const
{
  box.Import(m_position.x, m_position.y, m_halfWidth, m_halfHeight);
}


} // XR

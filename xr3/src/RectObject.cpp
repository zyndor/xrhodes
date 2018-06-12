//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/RectObject.hpp"

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

} // XR

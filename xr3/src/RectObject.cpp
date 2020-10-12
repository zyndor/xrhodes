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
: mPosition(pos),
  mHalfWidth(0.f),
  mHalfHeight(0.f)
{}

//==============================================================================
RectObject::RectObject(float hw, float hh, const Vector2& pos)
: mPosition(pos),
  mHalfWidth(hw),
  mHalfHeight(hh)
{}

//==============================================================================
RectObject::~RectObject()
{}

//==============================================================================
void  RectObject::SetHalfWidth(float hw)
{
  mHalfWidth = hw;
}

//==============================================================================
void  RectObject::SetHalfHeight(float hh)
{
  mHalfHeight = hh;
}

//==============================================================================
void  RectObject::Export(AABB& box) const
{
  box.Import(mPosition.x, mPosition.y, mHalfWidth, mHalfHeight);
}


} // XR

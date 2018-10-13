#ifndef XR_DEBUGDRAW_HPP
#define XR_DEBUGDRAW_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/math/Vector3.hpp"
#include "Material.hpp"

namespace xr
{
namespace DebugDraw
{

//==============================================================================
void  SetColor(Color const& color);

void  Line(const Vector3& v, Material::Ptr const& material = nullptr);
void  LineStrip(const Vector3* verts, int numVerts, Material::Ptr const& material = nullptr);
void  LineList(const Vector3* verts, int numVerts, Material::Ptr const& material = nullptr);

void  Rect(float hw, float hh, Material::Ptr const& material = nullptr);
void  FillRect(float hw, float hh, Material::Ptr const& material = nullptr);

void  Circle(float radius, Material::Ptr const& material = nullptr);
void  FillCircle(float radius, Material::Ptr const& material = nullptr);

//==============================================================================
// implementation
//==============================================================================

} // DebugDraw
} // XR

#endif // XR_DEBUGDRAW_HPP

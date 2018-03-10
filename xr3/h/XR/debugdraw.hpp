#ifndef XR_DEBUGDRAW_HPP
#define XR_DEBUGDRAW_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Vector3.hpp"
#include "Material.hpp"

namespace XR
{
namespace DebugDraw
{

//==============================================================================
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

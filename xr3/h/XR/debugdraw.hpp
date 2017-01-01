//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_DEBUGDRAW_HPP
#define XR_DEBUGDRAW_HPP

#include  "Renderer.hpp"

namespace XR
{

namespace DebugDraw
{

//==============================================================================
void  Line(const Vector3& v, Material* pMaterial = 0);
void  LineStrip(const Vector3* parVerts, int numVerts, Material* pMaterial = 0);
void  LineList(const Vector3* parVerts, int numVerts, Material* pMaterial = 0);

void  Rect(float hw, float hh, Material* pMaterial = 0);
void  FillRect(float hw, float hh, Material* pMaterial = 0);

void  Circle(float radius, Material* pMaterial = 0);
void  FillCircle(float radius, Material* pMaterial = 0);

//==============================================================================
// implementation
//==============================================================================

} // DebugDraw

} // XR

#endif // XR_DEBUGDRAW_HPP

//
// Nuclear Heart Games
// XRhodes
//
// debugdraw.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    09/07/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_DEBUGDRAW_HPP
#define XR_DEBUGDRAW_HPP

#include  "Renderer.hpp"

namespace XR
{

namespace DebugDraw
{

//==============================================================================
void  Line(const Vector3& v, Material* pMaterial = 0);
void  LineStrip(const Vector3* parVerts, int numVerts, Material* pMaterial = 0);

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

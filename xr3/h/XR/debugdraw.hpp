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
void  Rect(float hw, float hh, XR::Material* pMaterial = 0);
void  FillRect(float hw, float hh, XR::Material* pMaterial = 0);

void  Circle(float radius, XR::Material* pMaterial = 0);
void  FillCircle(float radius, XR::Material* pMaterial = 0);

//==============================================================================
// implementation
//==============================================================================

} // DebugDraw

} // XR

#endif // XR_DEBUGDRAW_HPP

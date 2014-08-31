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

//==============================================================================
void  DebugDrawRect(float hw, float hh);
void  DebugDrawFillRect(float hw, float hh);

void  DebugDrawCircle(float radius);
void  DebugDrawFillCircle(float radius);

//==============================================================================
// implementation
//==============================================================================

} // XR

#endif // XR_DEBUGDRAW_HPP

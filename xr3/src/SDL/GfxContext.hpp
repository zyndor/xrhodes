#ifndef XR_GFXCONTEXT_HPP
#define XR_GFXCONTEXT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/SVector2.hpp"
#include "SDL.h"

namespace xr
{
namespace Gfx
{

//==============================================================================
///@brief This wraps a GL context and its creation using SDL. The secondary
/// initialization - which the OpenGL implementation of Gfx requires - allows
/// calling it from a thread of the client's choice, designating this thread as
/// the rendering thread.
class Context
{
public:
  // structors
  Context(SDL_Window& window);
  ~Context();

  // general
  void Init();

  int GetGlVersionMajor() const
  {
    return m_glVersionMajor;
  }

  int GetGlVersionMinor() const
  {
    return m_glVersionMinor;
  }

  int GetLogicalWidth() const
  {
    return m_logicalSize.x;
  }

  int GetLogicalHeight() const
  {
    return m_logicalSize.y;
  }

  int GetPhysicalWidth() const
  {
    return m_physicalSize.x;
  }

  int GetPhysicalHeight() const
  {
    return m_physicalSize.y;
  }

  void Swap() const;

  void Shutdown();

private:
  // data
  SDL_Window& m_window; // no ownership
  SDL_Renderer* m_ownRenderer;
  SDL_GLContext m_ownContext;

  int m_glVersionMajor;
  int m_glVersionMinor;
  SVector2 m_physicalSize;
  SVector2 m_logicalSize;
};

} // Gfx
} // xr

#endif //XR_GFXCONTEXT_HPP

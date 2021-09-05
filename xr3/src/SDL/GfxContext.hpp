#ifndef XR_GFXCONTEXT_HPP
#define XR_GFXCONTEXT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Px.hpp"
#include "SDL2/SDL_render.h"

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

  int32_t GetGlVersionMajor() const
  {
    return m_glVersionMajor;
  }

  int32_t GetGlVersionMinor() const
  {
    return m_glVersionMinor;
  }

  Px GetLogicalWidth() const
  {
    return m_logicalSize.x;
  }

  Px GetLogicalHeight() const
  {
    return m_logicalSize.y;
  }

  Px GetPhysicalWidth() const
  {
    return m_physicalSize.x;
  }

  Px GetPhysicalHeight() const
  {
    return m_physicalSize.y;
  }

  void Swap() const;

  void Shutdown();

private:
  // types
  struct Size
  {
    Px x, y;
  };

  // data
  SDL_Window& m_window; // no ownership
  SDL_GLContext m_ownContext;

  int32_t m_glVersionMajor;
  int32_t m_glVersionMinor;
  Size m_physicalSize;
  Size m_logicalSize;
};

} // Gfx
} // xr

#endif //XR_GFXCONTEXT_HPP

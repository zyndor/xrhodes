//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "GfxContext.hpp"
#include "xr/Config.hpp"
#include "xr/debug.hpp"
#include "xr/types/intutils.hpp"
#include "SDL2/SDL_hints.h"

namespace xr
{
namespace Gfx
{
namespace
{
const int kDefaultGlMajorVersion = 3;
const int kDefaultGlMinorVersion = 3;
}

//==============================================================================
Gfx::Context::Context(SDL_Window& window)
: m_window(window),
  m_ownContext(nullptr)
{}

//==============================================================================
Context::~Context()
{
  Shutdown();
}

//==============================================================================
void Context::Init()
{
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

  // Create GL context.
  m_glVersionMajor = Config::GetInt("XR_GFX_GL_VERSION_MAJOR", kDefaultGlMajorVersion);
  m_glVersionMinor = Config::GetInt("XR_GFX_GL_VERSION_MINOR", kDefaultGlMinorVersion);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_glVersionMajor);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_glVersionMinor);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  int width, height;
  SDL_GL_GetDrawableSize(&m_window, &width, &height);

  m_ownContext = SDL_GL_CreateContext(&m_window);
  SDL_GL_MakeCurrent(&m_window, m_ownContext);

  // Get display and screen sizes.
  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  XR_ASSERT(GfxContext, Representable<Px>(dm.w));
  XR_ASSERT(GfxContext, Representable<Px>(dm.h));
  m_physicalSize = { Px(dm.w), Px(dm.h) };

  XR_ASSERT(GfxContext, Representable<Px>(width));
  XR_ASSERT(GfxContext, Representable<Px>(height));
  m_logicalSize = { Px(width), Px(height) };
}

//==============================================================================
void Context::Swap() const
{
  SDL_GL_SwapWindow(&m_window);
}

//==============================================================================
void Context::Shutdown()
{
  if (m_ownContext)
  {
    SDL_GL_MakeCurrent(nullptr, SDL_GLContext());

    SDL_GL_DeleteContext(m_ownContext);
    m_ownContext = nullptr;
  }
}

} // Gfx
} // xr

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "GfxContext.hpp"
#include "xr/Device.hpp"

namespace xr
{
namespace Gfx
{

//==============================================================================
Gfx::Context::Context(SDL_Window& window)
: m_window(window),
  m_ownRenderer(nullptr)
{}

//==============================================================================
Context::~Context()
{
  Shutdown();
}

//==============================================================================
void Context::Init()
{
  uint32_t flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
  if (Device::GetConfigInt("Display", "vsync", false))
  {
    flags |= SDL_RENDERER_PRESENTVSYNC;
  }

  m_ownRenderer = SDL_CreateRenderer(&m_window, -1, flags);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

  // Create GL context.
  m_glVersionMajor = 3;
  m_glVersionMinor = 3;

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_glVersionMajor);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_glVersionMinor);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  int width, height;
  SDL_GL_GetDrawableSize(&m_window, &width, &height);

  SDL_RenderSetLogicalSize(m_ownRenderer, width, height);

  m_ownContext = SDL_GL_CreateContext(&m_window);
  SDL_GL_MakeCurrent(&m_window, m_ownContext);

  // Get display and screen sizes.
  SDL_RendererInfo  info;
  SDL_GetRendererInfo(m_ownRenderer, &info);

  XR_ASSERT(Gfx, info.flags & SDL_RENDERER_ACCELERATED);
  XR_ASSERT(Gfx, info.flags & SDL_RENDERER_TARGETTEXTURE);

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  m_physicalSize = SVector2(dm.w, dm.h);
  m_logicalSize = SVector2(width, height);
}

//==============================================================================
void Context::Swap() const
{
  SDL_GL_SwapWindow(&m_window);
}

//==============================================================================
void Context::Shutdown()
{
  if (m_ownRenderer)
  {
    SDL_GL_MakeCurrent(nullptr, SDL_GLContext());

    SDL_GL_DeleteContext(m_ownContext);
    SDL_DestroyRenderer(m_ownRenderer);
    m_ownRenderer = nullptr;
  }
}

} // Gfx
} // xr

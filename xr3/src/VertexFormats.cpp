//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/VertexFormats.hpp"

namespace xr
{
namespace Vertex
{
namespace
{
static bool s_registeredOnShutdown = false;
}

//==============================================================================
void Formats::HandleHolder::EnsureOnExitRegistration()
{
  if (!s_registeredOnShutdown)
  {
    auto onExit = [](void*, void*) {
      ForEach([](HandleHolder& hh) {
        Gfx::Release(hh.value);
        hh.value.Invalidate();
      });
      s_registeredOnShutdown = false; // Gfx is torn down.
    };
    Gfx::RegisterShutdownCallback(onExit, nullptr);
    s_registeredOnShutdown = true;
  }
}

//==============================================================================
Formats::HandleHolder::HandleHolder()
: Linked<HandleHolder>(*this)
{}

}
}

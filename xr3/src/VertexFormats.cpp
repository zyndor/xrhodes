//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/VertexFormats.hpp"

namespace XR
{
namespace Vertex
{
namespace
{
static bool s_registeredOnExit = false;
}

//==============================================================================
void Formats::HandleHolder::EnsureOnExitRegistration()
{
  if (!s_registeredOnExit)
  {
    auto onExit = [](void*, void*) {
      ForEach([](HandleHolder& hh) {
        Gfx::Release(hh.value);
        hh.value.Invalidate();
      });
      s_registeredOnExit = false; // Gfx is torn down.
    };
    Gfx::RegisterExitCallback(onExit, nullptr);
    s_registeredOnExit = true;
  }
}

//==============================================================================
Formats::HandleHolder::HandleHolder()
: Linked<HandleHolder>(*this)
{}

}
}

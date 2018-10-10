//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
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
    auto onExit = [](void*) {
      ForEach([](HandleHolder& hh) {
        Gfx::Release(hh.value);
        hh.value.Invalidate();
      });
      s_registeredOnShutdown = false; // Gfx is torn down.
    };
    Gfx::ShutdownSignal().Connect(FunctionPtrCallback<void>(onExit, nullptr));
    s_registeredOnShutdown = true;
  }
}

//==============================================================================
Formats::HandleHolder::HandleHolder()
: Linked<HandleHolder>(*this)
{}

}
}

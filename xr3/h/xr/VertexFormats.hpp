#ifndef XR_VERTEXFORMATS_HPP
#define XR_VERTEXFORMATS_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Vertex.hpp"
#include "Gfx.hpp"
#include "xr/fundamentals.hpp"
#include "xr/Linked.hpp"

namespace xr
{
namespace Vertex
{

///@brief Acts as a central repository and manager for VertexFormatHandles.
class Formats
{
  XR_NONOBJECT_DECL(Formats)

public:
  // static
  ///@brief Retrieves, and if need be creates the Gfx handle for the given vertex
  /// format.
  template <typename Format>
  static Gfx::VertexFormatHandle GetHandle()
  {
    static HandleHolder handle;
    if (!handle.value.IsValid())
    {
      HandleHolder::EnsureOnExitRegistration();
      handle.value = Format::Register();
    }
    return handle.value;
  }

private:
  struct HandleHolder: Linked<HandleHolder>
  {
    static void EnsureOnExitRegistration();

    Gfx::VertexFormatHandle value;

    HandleHolder();
  };
};

}
}
#endif // XR_VERTEXFORMATS_HPP

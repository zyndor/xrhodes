#ifndef XR_PRIMITIVE_HPP
#define XR_PRIMITIVE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

namespace xr
{

//==============================================================================
///@brief Drawing primitives supported by XRhodes.
enum class Primitive
{
  PointList,
  LineList,
  LineStrip,
  TriangleList,
  TriangleStrip,

  // deprecated
  TriList,
  TriStrip,
};

} // xr

#endif // XR_PRIMITIVE_HPP

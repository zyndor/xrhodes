#ifndef XR_DEBUGDRAW_HPP
#define XR_DEBUGDRAW_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/math/Vector3.hpp"
#include "Material.hpp"

namespace xr
{
namespace DebugDraw
{

//==============================================================================
///@brief Sets the color of the default material that may be used by the debug
/// drawing API (accessible outside as the uniform named xruColor).
void  SetColor(Color const& color);

///@brief Sets the state that shader of the default material will use.
void  SetState(Gfx::FlagType clear, Gfx::FlagType set);

///@brief Draws a line from the current transform to the position @a v, with the
/// given material (if null, a default material will be used).
void  Line(const Vector3& v, Material::Ptr const& material = nullptr);

///@brief Draws a line strip using the first @a numVerts positions in @a verts,
/// with the given material (if null, a default material will be used).
void  LineStrip(const Vector3* verts, int numVerts, Material::Ptr const& material = nullptr);

///@brief Draws a line list using the first @a numVerts positions in @a verts,
/// with the given material (if null, a default material will be used).
void  LineList(const Vector3* verts, int numVerts, Material::Ptr const& material = nullptr);

///@brief Draws a rectangle of half width @a hw and half height @a hh, centered
/// on the current transform, with the given @a material (if null, a
/// default material will be used).
void  Rect(float hw, float hh, Material::Ptr const& material = nullptr);

///@brief Draws a filled rectangle of half width @a hw and half height @a hh,
/// centered on the current transform, with the given @a material (if null, a
/// default material will be used).
void  FillRect(float hw, float hh, Material::Ptr const& material = nullptr);

///@brief Draws a circle of half width @a hw and half height @a hh, centered
/// on the current transform, with the given @a material (if null, a
/// default material will be used).
void  Circle(float radius, Material::Ptr const& material = nullptr);

///@brief Draws a filled circle of half width @a hw and half height @a hh,
/// centered on the current transform, with the given @a material (if null, a
/// default material will be used).
void  FillCircle(float radius, Material::Ptr const& material = nullptr);

} // DebugDraw
} // xr

#endif // XR_DEBUGDRAW_HPP

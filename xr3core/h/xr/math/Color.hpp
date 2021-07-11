#ifndef XR_COLOR_HPP
#define XR_COLOR_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "mathutils.hpp"
#include "xr/debug.hpp"
#include <cstdint>

namespace xr
{

//==============================================================================
enum
{
  kGreenShift = 8,
  kBlueShift = 16,
  kAlphaShift = 24
};

//==============================================================================
class Color
{
public:
  // types
  enum  Component
  {
    R,
    G,
    B,
    A,
    kNumComponents
  };

  // static
  static Color  s_default;

  static constexpr float ByteToNormalized(uint8_t value)
  {
    return value * kRecByteMax;
  }

  // data
  float  r, g, b, a;

  // structors
  constexpr Color()
  : r(.0f), g(.0f), b(.0f), a(.0f)
  {}

  constexpr Color(float r_, float g_, float b_, float a_)
  : r(r_),
    g(g_),
    b(b_),
    a(a_)
  {}

  constexpr Color(float r_, float g_, float b_)
  : Color(r_, g_, b_, 1.0f)
  {}

  explicit constexpr Color(const float data[kNumComponents])
  : Color(data[R], data[G], data[B], data[A])
  {}

  explicit constexpr Color(uint32_t abgr)
  : Color(ByteToNormalized(abgr & 0xff),
      ByteToNormalized((abgr >> kGreenShift) & 0xff),
      ByteToNormalized((abgr >> kBlueShift) & 0xff),
      ByteToNormalized((abgr >> kAlphaShift) & 0xff))
  {}

  // general
  uint32_t  GetABGR() const
  {
    return uint8_t(a * 0xff) << kAlphaShift |
      uint8_t(b * 0xff) << kBlueShift |
      uint8_t(g * 0xff) << kGreenShift |
      uint8_t(r * 0xff);
  }

  void  Set(uint32_t abgr)
  {
    r = ByteToNormalized(abgr & 0xff);
    g = ByteToNormalized((abgr >> kGreenShift) & 0xff);
    b = ByteToNormalized((abgr >> kBlueShift) & 0xff);
    a = ByteToNormalized((abgr >> kAlphaShift) & 0xff);
  }

  void  Set(float r_, float g_, float b_, float a_)
  {
    r = r_;
    g = g_;
    b = b_;
    a = a_;
  }

  Color&  Clamp()
  {
    r = Saturate(r);
    g = Saturate(g);
    b = Saturate(b);
    a = Saturate(a);
    return *this;
  }

  Color Solid() const
  {
    return Color(r, g, b);
  }

  // range based for support
  float* begin()
  {
    return &r;
  }

  float* end()
  {
    return &r + kNumComponents;
  }

  float const* begin() const
  {
    return &r;
  }

  float const* end() const
  {
    return &r + kNumComponents;
  }

  // operator overloads
  Color&  operator +=(const Color& rhs)
  {
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    a += rhs.a;
    return *this;
  }

  Color&  operator -=(const Color& rhs)
  {
    r -= rhs.r;
    g -= rhs.g;
    b -= rhs.b;
    a -= rhs.a;
    return *this;
  }

  Color&  operator *=(float s)
  {
    r *= s;
    g *= s;
    b *= s;
    a *= s;
    return *this;
  }

  Color&  operator /=(float s)
  {
    XR_ASSERT(Color, s != .0f);
    s = 1.0f / s;
    r *= s;
    g *= s;
    b *= s;
    a *= s;
    return *this;
  }

  Color operator +(const Color& rhs) const
  {
    Color temp(*this);
    return temp += rhs;
  }

  Color operator -(const Color& rhs) const
  {
    Color temp(*this);
    return temp -= rhs;
  }

  Color operator *(float s) const
  {
    return Color(r * s, g * s, b * s, a * s);
  }

  Color operator /(float s) const
  {
    XR_ASSERT(Color, s != .0f);
    s = 1.0f / s;
    return Color(r * s, g * s, b * s, a * s);
  }
};

//==============================================================================
//  implementation
//==============================================================================


} // XR

//==============================================================================
#define XR_ABGR_ALPHA(a) ((a) << xr::kAlphaShift)
#define XR_ABGR_BLUE(b) ((b) << xr::kBlueShift)
#define XR_ABGR_GREEN(g) ((g) << xr::kGreenShift)

#define XR_ABGR_EXTRACT_ALPHA(abgr) (((abgr) & (0xff << xr::kAlphaShift)) >> xr::kAlphaShift)
#define XR_ABGR_EXTRACT_BLUE(abgr) (((abgr) & (0xff << xr::kBlueShift)) >> xr::kBlueShift)
#define XR_ABGR_EXTRACT_GREEN(abgr) (((abgr) & (0xff << xr::kGreenShift)) >> xr::kGreenShift)
#define XR_ABGR_EXTRACT_RED(abgr)  ((abgr) & 0xff)

#define XR_RGBA_EXTRACT_RED(rgba) (XR_ABGR_EXTRACT_ALPHA(rgba))
#define XR_RGBA_EXTRACT_GREEN(rgba) (XR_ABGR_EXTRACT_BLUE(rgba))
#define XR_RGBA_EXTRACT_BLUE(rgba) (XR_ABGR_EXTRACT_GREEN(rgba))
#define XR_RGBA_EXTRACT_ALPHA(rgba) (XR_ABGR_EXTRACT_RED(rgba))

#endif // XR_COLOR_HPP

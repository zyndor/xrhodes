//
// Nuclear Heart Games
// XRhodes
//
// Color.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    22/03/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_COLOR_HPP
#define XR_COLOR_HPP

#include "types.hpp"

namespace XR
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
  
  // data
  union
  {
    struct 
    {
      float  r, g, b, a;
    };
    float arData[kNumComponents];
  };

  // structors
  Color()
  : r(.0f), g(.0f), b(.0f), a(.0f)
  {}
  
  Color(float r_, float g_, float b_, float a_)
  : r(r_),
    g(g_),
    b(b_),
    a(a_)
  {}
  
  Color(float r_, float g_, float b_)
  : r(r_),
    g(g_),
    b(b_),
    a(1.0f)
  {}
  
  explicit Color(const float arData[kNumComponents])
  : r(arData[R]),
    g(arData[G]),
    b(arData[B]),
    a(arData[A])
  {}

  explicit Color(uint32 abgr)
  : r((abgr & 0xff) * XR_ONE_OVER_0XFF),
    g(((abgr >> kGreenShift) & 0xff) * XR_ONE_OVER_0XFF),
    b(((abgr >> kBlueShift) & 0xff) * XR_ONE_OVER_0XFF),
    a(((abgr >> kAlphaShift) & 0xff) * XR_ONE_OVER_0XFF)
  {}

  // general
  uint32  GetABGR() const
  {
    return uint8(a * 0xff) << kAlphaShift |
      uint8(b * 0xff) << kBlueShift |
      uint8(g * 0xff) << kGreenShift |
      uint8(r * 0xff);
  }

  void  Set(uint32 abgr)
  {
    r = (abgr & 0xff) * XR_ONE_OVER_0XFF;
    g = ((abgr >> kGreenShift) & 0xff) * XR_ONE_OVER_0XFF;
    b = ((abgr >> kBlueShift) & 0xff) * XR_ONE_OVER_0XFF;
    a = ((abgr >> kAlphaShift) & 0xff) * XR_ONE_OVER_0XFF;
  }

  // overloads
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
#define XR_ABGR_ALPHA(a) ((a) << XR::kAlphaShift)
#define XR_ABGR_BLUE(b) ((b) << XR::kBlueShift)
#define XR_ABGR_GREEN(g) ((g) << XR::kGreenShift)

#define XR_ABGR_EXTRACT_ALPHA(abgr) (((abgr) & (0xff << XR::kAlphaShift)) >> XR::kAlphaShift)
#define XR_ABGR_EXTRACT_BLUE(abgr) (((abgr) & (0xff << XR::kBlueShift)) >> XR::kBlueShift)
#define XR_ABGR_EXTRACT_GREEN(abgr) (((abgr) & (0xff << XR::kGreenShift)) >> XR::kGreenShift)
#define XR_ABGR_EXTRACT_RED(abgr)  ((abgr) & 0xff)

#define XR_RGBA_EXTRACT_RED(rgba) (XR_ABGR_EXTRACT_ALPHA(rgba))
#define XR_RGBA_EXTRACT_GREEN(rgba) (XR_ABGR_EXTRACT_BLUE(rgba))
#define XR_RGBA_EXTRACT_BLUE(rgba) (XR_ABGR_EXTRACT_GREEN(rgba))
#define XR_RGBA_EXTRACT_ALPHA(rgba) (XR_ABGR_EXTRACT_RED(rgba))

#endif // XR_COLOR_HPP

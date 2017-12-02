#ifndef XR_VERTEX_HPP
#define XR_VERTEX_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Gfx.hpp"
#include "Vector3.hpp"

namespace XR
{

namespace Vertex
{

//==============================================================================
///@brief A vertex position component for a Vertex::Format<>.
template <typename V>
struct Pos
{
  enum { kType = int(Gfx::Attribute::Position),
    kCount = V::kNumComponents,
    kNormalize = false };
  static_assert(kCount >= 2 && kCount <= 4, "Invalid Pos size");

  V pos;

  Pos(V const& v = Vector3())
  : pos(v)
  {}
};

//==============================================================================
///@brief One of the 8 supported UV components for a Vertex::Format<>.
#define XR_UV_GENERATOR_(i)\
  template <typename V> \
  struct UV##i \
  {\
    enum { kType = int(Gfx::Attribute::UV##i),\
      kCount = V::kNumComponents,\
      kNormalize = false };\
    static_assert(kCount >= 2 && kCount <= 3, "Invalid UV"#i" size");\
    UV##i(V const& v = V())\
    : uv##i(v)\
    {}\
    V uv##i;\
  };

#define XR_UV_GENERATOR(i) XR_UV_GENERATOR_(i)

XR_UV_GENERATOR(0)
XR_UV_GENERATOR(1)
XR_UV_GENERATOR(2)
XR_UV_GENERATOR(3)
XR_UV_GENERATOR(4)
XR_UV_GENERATOR(5)
XR_UV_GENERATOR(6)
XR_UV_GENERATOR(7)
#undef XR_UV_GENERATOR_
#undef XR_UV_GENERATOR

//==============================================================================
///@brief The Normal component for a Vertex::Format<>.
struct Normal
{
  enum { kType = int(Gfx::Attribute::Normal),
    kCount = 3,
    kNormalize = true };

  Vector3 normal;

  Normal(Vector3 const& v = Vector3())
  : normal(v)
  {}
};

//==============================================================================
///@brief One of the 2 supported Color components for a Vertex::Format<>.
#define XR_COLOR_GENERATOR_(i)\
  template <typename V> \
  struct Color##i \
  {\
    enum { kType = int(Gfx::Attribute::Color##i),\
      kCount = V::kNumComponents,\
      kNormalize = false };\
    static_assert(kCount >= 3 && kCount <= 4, "Invalid Color"#i" size");\
    V color##i;\
    Color##i(V const& v = V())\
    : color##i(v)\
    {}\
  };

#define XR_COLOR_GENERATOR(i) XR_COLOR_GENERATOR_(i)

XR_COLOR_GENERATOR(0)
XR_COLOR_GENERATOR(1)
#undef XR_COLOR_GENERATOR_
#undef XR_COLOR_GENERATOR

//==============================================================================
///@brief The Tangent component for a Vertex::Format<>.
struct Tangent
{
  enum { kType = int(Gfx::Attribute::Tangent), kCount = 3, kNormalize = true };
  Vector3 tangent;

  Tangent(Vector3 const& v = Vector3())
  : tangent(v)
  {}
};

//==============================================================================
///@brief The Bitangent component for a Vertex::Format<>.
struct Bitangent
{
  enum { kType = int(Gfx::Attribute::Bitangent), kCount = 3, kNormalize = true };
  Vector3 bitangent;

  Bitangent(Vector3 const& v = Vector3())
  : bitangent(v)
  {}
};

//==============================================================================
///@brief Facilitates the creation and registration of a compile-time Gfx vertex
/// format, using a variation of the types above.
///@note A warning of a repeated base class is a precursor of a runtime failure
/// attempting to defining one component more than once.
template <class... Rest>
struct Format
{
  static void AddAttribute(Gfx::VertexFormat&)
  {}
};

template <class T, class... Rest>
struct Format<T, Rest...> : T, Format<Rest...>
{
  using Self = T;
  
  Format()
  : T(),
    Format<Rest...>()
  {}

  Format(T t, Rest... r)
  : T(t),
    Format<Rest...>(r...)
  {}

  static Gfx::VertexFormatHandle Register()
  {
    Gfx::VertexFormat vf;
    AddAttribute(vf);
    return Gfx::RegisterVertexFormat(vf);
  }

  static void AddAttribute(Gfx::VertexFormat& vf)
  {
    vf.Add(static_cast<Gfx::Attribute>(Self::kType), Self::kCount, Self::kNormalize);
    Format<Rest...>::AddAttribute(vf);
  }
};

}
}

#endif
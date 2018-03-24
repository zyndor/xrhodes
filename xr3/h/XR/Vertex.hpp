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
#include "Color.hpp"

namespace XR
{
namespace Vertex
{

//==============================================================================
///@brief A vertex position component for a Vertex::Format<>.
template <typename V = Vector3>
struct Pos
{
  using Value = V;

  enum { kType = int(Gfx::Attribute::Position),
    kCount = V::kNumComponents,
    kNormalize = false };
  static_assert(kCount >= 2 && kCount <= 4, "Invalid Pos size");

  V pos;

  Pos(V const& v = V())
  : pos(v)
  {}
};

//==============================================================================
///@brief One of the 8 supported UV components for a Vertex::Format<>.
#define XR_UV_GENERATOR_(i)\
  template <typename V = Vector2> \
  struct UV##i \
  {\
    using Value = V;\
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
///@brief One of the 2 supported Color components for a Vertex::Format<>.
#define XR_COLOR_GENERATOR_(i)\
  template <typename V> \
  struct Color##i \
  {\
    using Value = V;\
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
///@brief Components that have Vector3 data and are normalized.
#define XR_NORMALIZED_VECTOR3_GENERATOR(typeName, memberName)\
  struct typeName\
  {\
    using Value = Vector3;\
    enum {\
      kType = int(Gfx::Attribute::typeName),\
      kCount = Value::kNumComponents,\
      kNormalize = true\
    };\
    Vector3 memberName;\
    typeName(Vector3 const& v = Vector3())\
    : memberName(v)\
    {}\
  };

///@brief A 3-dimensional normal component for a Vertex::Format<>.
XR_NORMALIZED_VECTOR3_GENERATOR(Normal, normal)

///@brief A 3-dimensional tangent component for a Vertex::Format<>.
XR_NORMALIZED_VECTOR3_GENERATOR(Tangent, tangent)

///@brief A 3-dimensional bitangent component for a Vertex::Format<>.
XR_NORMALIZED_VECTOR3_GENERATOR(Bitangent, bitangent)
#undef XR_NORMALIZED_VECTOR3_GENERATOR

//==============================================================================
///@brief Facilitates the creation and registration of a compile-time vertex
/// format for Gfx, using a combination of the types above. It also allows
/// specialising previously declared vertex formats.
///@note A warning of a repeated base class is an indication of component being
/// used more than once, leading to a runtime failure.
template <class... Rest>
struct Format
{
  enum { kSize = 0 };
  static void AddAttribute(Gfx::VertexFormat&)
  {}
};

template <class T, class... Rest>
struct Format<T, Rest...> : T, Format<Rest...>
{
  // types
  using Base = Format<Rest...>;
  using Self = Format<T, Rest...>;
  using Value = typename T::Value;

  enum { kSize = sizeof(T) + Base::kSize };

  // static
  static Gfx::VertexFormatHandle Register()
  {
    Gfx::VertexFormat vf;
    AddAttribute(vf);
    return Gfx::RegisterVertexFormat(vf);
  }

  // structors
  Format()
  : T(),
    Base()
  {}

  template <class... RestT>
  Format(Value v = Value(), RestT... r)
  : T(v),
    Base(r...)
  {}

  // operator overloads
  using Base::operator=;

  Self& operator=(T const& rhs)
  {
    Self& self = *this;
    static_cast<T&>(self) = rhs;
    return self;
  }

protected:
  // static
  static void AddAttribute(Gfx::VertexFormat& vf)
  {
    vf.Add(static_cast<Gfx::Attribute>(T::kType), T::kCount, T::kNormalize);
    Base::AddAttribute(vf);
  }
};

template <class T, class... Rest>
struct Format<T, Format<Rest...>> : Format<Rest...>, T
{
  // types
  using Base = Format<Rest...>;
  using Self = Format<T, Base>;
  using Value = typename T::Value;

  enum { kSize = sizeof(T) + Base::kSize };

  // static
  static Gfx::VertexFormatHandle Register()
  {
    Gfx::VertexFormat vf;
    AddAttribute(vf);
    return Gfx::RegisterVertexFormat(vf);
  }

  // structors
  Format()
  : Base(),
    T()
  {}

  template <class... RestT>
  Format(Value v = Value(), RestT... r)
  : T(v),
    Base(r...)
  {}

  // operator overloads
  using Base::operator=;

  Self& operator=(T const& rhs)
  {
    Self& self = *this;
    static_cast<T&>(self) = rhs;
    return self;
  }

protected:
  // static
  static void AddAttribute(Gfx::VertexFormat& vf)
  {
    Base::AddAttribute(vf);
    vf.Add(static_cast<Gfx::Attribute>(T::kType), T::kCount, T::kNormalize);
  }
};

} // Vertex
}

#endif

#ifndef XR_BASICMESH_HPP
#define XR_BASICMESH_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "FloatBuffer.hpp"
#include "Material.hpp"
#include "VertexFormats.hpp"

namespace XR {

//==============================================================================
class BasicMeshCore
{
public:
  // structors
  BasicMeshCore(uint32_t vertexSize, uint32_t numVertices = 0);
  ~BasicMeshCore();

  // general
  Material::Ptr GetMaterial() const;
  void  SetMaterial(Material::Ptr const& material);

  ///@brief Renders the mesh as the given primitive type, without setting the
  /// material.
  void RenderOnly(PrimType primitiveType) const;

  ///@brief Renders @a count vertices from @a offset, of the mesh, as the given
  /// primitive type, without setting the material.
  void RenderOnly(PrimType primitiveType, uint32_t offset, uint32_t count) const;

protected:
  // data
  Material::Ptr m_material;
  FloatBuffer m_vertices;
  Gfx::VertexBufferHandle m_vbo;
};

//==============================================================================
template <class VertexFormat>
class BasicMesh: protected BasicMeshCore
{
public:
  // types
  using PosType = decltype(VertexFormat::pos);

  // structors
  BasicMesh(uint32_t numVertices = 0);

  // general
  using BasicMeshCore::GetMaterial;
  using BasicMeshCore::SetMaterial;
  using BasicMeshCore::RenderOnly;

  void AllocBuffer(uint32_t numVertices);

  Buffer GetVertexBuffer() const;

  VertexFormat* GetVertices();
  const VertexFormat* GetVertices() const;

  ///@brief Creates a VBO using its vertex data, and storing the resulting
  /// handle internally (the previously held handle will be deleted).
  void CreateVbo(uint32_t flags = Gfx::F_BUFFER_NONE);

  PosType CalculateCentre() const;

  float CalculateRadius() const;
  void CalculateExtents(PosType& minOut, PosType& maxOut) const;
};

//==============================================================================
// implementation
//==============================================================================
inline
Material::Ptr BasicMeshCore::GetMaterial() const
{
  return m_material;
}

//==============================================================================
inline
void BasicMeshCore::RenderOnly(PrimType primitiveType) const
{
  RenderOnly(primitiveType, 0, m_vertices.GetNumElements());
}

//==============================================================================
inline
void BasicMeshCore::RenderOnly(PrimType primitiveType, uint32_t offset, uint32_t count) const
{
  XR_ASSERT(BasicMesh, m_vbo.IsValid());
  Gfx::Draw(m_vbo, primitiveType, offset, count);
}

//==============================================================================
template <class VertexFormat>
inline
void BasicMesh<VertexFormat>::CreateVbo(uint32_t flags)
{
  Gfx::Destroy(m_vbo);
  XR_ASSERT(BasicMesh, m_vertices.GetRaw() != nullptr);
  m_vbo = Gfx::CreateVertexBuffer(Vertex::Formats::GetHandle<VertexFormat>(),
    GetVertexBuffer(), flags);
}

//==============================================================================
template <class VertexFormat>
BasicMesh<VertexFormat>::BasicMesh(uint32_t numVertices)
: BasicMeshCore(VertexFormat::kSize, numVertices)
{}

//==============================================================================
template<class VertexFormat>
inline
VertexFormat* BasicMesh<VertexFormat>::GetVertices()
{
  return m_vertices.Get<VertexFormat>();
}

//==============================================================================
template<class VertexFormat>
inline
const VertexFormat* BasicMesh<VertexFormat>::GetVertices() const
{
  return m_vertices.Get<VertexFormat>();
}

//==============================================================================
template <class VertexFormat>
void  BasicMesh<VertexFormat>::AllocBuffer(uint32_t numVertices)
{
  XR_ASSERT(BasicMesh, numVertices >= 0);
  m_vertices.SetBuffer(VertexFormat::kSize, numVertices);
}

//==============================================================================
template<class VertexFormat>
inline Buffer BasicMesh<VertexFormat>::GetVertexBuffer() const
{
  return Buffer{ m_vertices.GetElementSizeBytes() * m_vertices.GetNumElements(),
    reinterpret_cast<uint8_t const*>(m_vertices.GetRaw()) };
}

//==============================================================================
template <class VertexFormat>
typename BasicMesh<VertexFormat>::PosType
  BasicMesh<VertexFormat>::CalculateCentre() const
{
  PosType centre = PosType::Zero();
  const auto n = m_vertices.GetNumElements();
  if (n > 0)
  {
    for (auto i0 = GetVertices(), i1 = i0 + n; i0 != i1; ++i0)
    {
      centre += i0->pos;
    }
    centre *= 1.0f / n;
  }
  return centre;
}

//==============================================================================
template <class VertexFormat>
float BasicMesh<VertexFormat>::CalculateRadius() const
{
  float radius = .0f;
  const auto n = m_vertices.GetNumElements();
  if (n > 0)
  {
    for (auto i0 = GetVertices(), i1 = i0 + n; i0 != i1; ++i0)
    {
      float d = i0->pos.Dot();
      if (d > radius)
      {
        radius = d;
      }
    }
    radius = sqrtf(radius);
  }
  return radius;
}

//==============================================================================
template <class VertexFormat>
void  BasicMesh<VertexFormat>::CalculateExtents(PosType& minOut, PosType& maxOut) const
{
  const auto n = m_vertices.GetNumElements();
  if (n > 0)
  {
    for (int i = 0; i < PosType::kNumComponents; ++i)
    {
      minOut.arData[i] = std::numeric_limits<float>::max();
      maxOut.arData[i] = std::numeric_limits<float>::min();
    }

    for (auto i0 = GetVertices(), i1 = i0 + n; i0 != i1; ++i0)
    {
      for (int j = 0; j < PosType::kNumComponents; ++j)
      {
        if (i0->pos.arData[j] < minOut.arData[j])
        {
          minOut.arData[j] = i0->pos.arData[j];
        }

        if (i0->pos.arData[j] > maxOut.arData[j])
        {
          minOut.arData[j] = i0->pos.arData[j];
        }
      }
    }
  }
  else
  {
    minOut = maxOut = PosType::Zero();
  }
}

} // XR

#endif // XR_BASICMESH_HPP

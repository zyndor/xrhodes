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

namespace XR {

//==============================================================================
class BasicMesh
{
public:
  // structors
  BasicMesh();
  BasicMesh(int numVertices);
  virtual ~BasicMesh();
  
  // general
  FloatBuffer&        GetVertices();
  const FloatBuffer&  GetVertices() const;

  FloatBuffer&        GetUVs();
  const FloatBuffer&  GetUVs() const;
  
  Material::Ptr       GetMaterial() const;
  void                SetMaterial(Material::Ptr const& material); // no transfer
  
  void                InitStreams(int numVertices);
  
  Vector3             CalculateCentre() const;
  float               CalculateRadius() const;
  void                CalculateExtents(Vector3& minOut, Vector3& maxOut) const;
  
  virtual void        Render() const;
  
protected:
  // data
  Material::Ptr m_material;  // no ownership
  FloatBuffer   m_vertices;
  FloatBuffer   m_uvs;
  
  // internal
  void  _PrepareRender() const;
};

//==============================================================================
// implementation
//==============================================================================
inline
FloatBuffer& BasicMesh::GetVertices()
{
  return  m_vertices;
}

//==============================================================================
inline
const FloatBuffer& BasicMesh::GetVertices() const
{
  return  m_vertices;
}

//==============================================================================
inline
FloatBuffer& BasicMesh::GetUVs()
{
  return  m_uvs;
}

//==============================================================================
inline
const FloatBuffer& BasicMesh::GetUVs() const
{
  return  m_uvs;
}

//==============================================================================
inline
Material::Ptr BasicMesh::GetMaterial() const
{
  return m_material;
}

} // XR

#endif // XR_BASICMESH_HPP

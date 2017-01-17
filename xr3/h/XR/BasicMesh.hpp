//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_BASICMESH_HPP
#define XR_BASICMESH_HPP

#include "RenderStream.hpp"
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
  RenderStream&       GetVertices();
  const RenderStream& GetVertices() const;

  RenderStream&       GetUVs();
  const RenderStream& GetUVs() const;
  
  Material*           GetMaterial() const;
  void                SetMaterial(Material* pMaterial); // no transfer
  
  void                InitStreams(int numVertices);
  
  Vector3             CalculateCentre() const;
  float               CalculateRadius() const;
  void                CalculateExtents(Vector3& minOut, Vector3& maxOut) const;
  
  virtual void        Render(); //const?
  
protected:
  // data
  Material*     m_pMaterial;  // no ownership
  RenderStream  m_vertices;
  RenderStream  m_uvs;
  
  // internal
  void  _PrepareRender();
};

//==============================================================================
// implementation
//==============================================================================
inline
RenderStream& BasicMesh::GetVertices()
{
  return  m_vertices;
}

//==============================================================================
inline
const RenderStream& BasicMesh::GetVertices() const
{
  return  m_vertices;
}

//==============================================================================
inline
RenderStream& BasicMesh::GetUVs()
{
  return  m_uvs;
}

//==============================================================================
inline
const RenderStream& BasicMesh::GetUVs() const
{
  return  m_uvs;
}

//==============================================================================
inline
Material* BasicMesh::GetMaterial() const
{
  return m_pMaterial;
}

} // XR

#endif // XR_BASICMESH_HPP

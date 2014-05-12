//
// Nuclear Heart Games
// XRhodes
//
// BasicMesh.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    04/02/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_BASICMESH_HPP
#define XR_BASICMESH_HPP

#include <vector>
#include "Renderer.hpp"

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
//  implementation
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

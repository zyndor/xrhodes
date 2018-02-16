#ifndef XR_MESHRENDERER_HPP
#define XR_MESHRENDERER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "Entity.hpp"
#include "IndexMesh.hpp"
#include "Vertex.hpp"

namespace XR
{

//==============================================================================
class MeshRenderer:  public Entity::ComponentT<MeshRenderer>
{
public:
  // types
  using VertexFormat = Vertex::Format<Vertex::Pos<Vector3>,
    Vertex::UV0<Vector2>,
    Vertex::Normal,
    Vertex::Tangent,
    Vertex::Bitangent>;

  // structors
  MeshRenderer();
  ~MeshRenderer();
  
  // general
  virtual MeshRenderer*  Clone() const;
  
  IndexMesh<VertexFormat>*  GetMesh() const;  // no ownership transfer
  void        SetMesh(IndexMesh<VertexFormat>* pMesh);  // no ownership transfer
  
  void        Render();
  
protected:
  // data
  IndexMesh<VertexFormat>*  m_pMesh;  // no ownership
};

//==============================================================================
// implementation
//==============================================================================
inline
IndexMesh<MeshRenderer::VertexFormat>*  MeshRenderer::GetMesh() const
{
  return  m_pMesh;
}

} // XR

#endif // XR_MESHRENDERER_HPP

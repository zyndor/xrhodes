//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include  "BasicMesh.hpp"

namespace XR {

//==============================================================================
BasicMesh::BasicMesh()
: m_vertices(),
  m_uvs()
{}

//==============================================================================
BasicMesh::BasicMesh(int numVertices)
: m_vertices(RenderStream::F_VECTOR3, numVertices),
  m_uvs(RenderStream::F_VECTOR2, numVertices)
{}

//==============================================================================
BasicMesh::~BasicMesh()
{}

//==============================================================================
void  BasicMesh::InitStreams(int numVertices)
{
  XR_ASSERT(BasicMesh, numVertices >= 0);
  m_vertices.Create(RenderStream::F_VECTOR3, numVertices);
  m_uvs.Create(RenderStream::F_VECTOR2, numVertices);
}

//==============================================================================
void  BasicMesh::Render()
{
  if(m_vertices.GetCapacity() > 0)
  {
    _PrepareRender();
    Renderer::DrawPrims(PRIM_TRI_STRIP);
  }
}

//==============================================================================
void  BasicMesh::_PrepareRender()
{
  XR_ASSERT(BasicMesh, m_pMaterial != 0);
  XR_ASSERT(BasicMesh, m_vertices.GetCapacity() > 0);
  Renderer::SetMaterial(m_pMaterial);
  
  Renderer::SetUVStream(m_uvs, 0);
  Renderer::SetVertStream(m_vertices);
}

} // XR

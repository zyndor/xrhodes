//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "BasicMesh.hpp"
#include "Renderer.hpp"

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
void  BasicMesh::SetMaterial(Material* pMaterial)
{
  m_pMaterial = pMaterial;
}

//==============================================================================
void  BasicMesh::InitStreams(int numVertices)
{
  XR_ASSERT(BasicMesh, numVertices >= 0);
  m_vertices.Create(RenderStream::F_VECTOR3, numVertices);
  m_uvs.Create(RenderStream::F_VECTOR2, numVertices);
}

//==============================================================================
Vector3 BasicMesh::CalculateCentre() const
{
  Vector3 centre(Vector3::Zero());
  const int n(m_vertices.GetCapacity());
  if(n > 0)
  {
    for(int i = 0; i < n; ++i)
    {
      centre += m_vertices.GetVector3(i);
    }
    centre *= 1.0f / n;
  }
  return centre;
}

//==============================================================================
float BasicMesh::CalculateRadius() const
{
  float radius(.0f);
  const int n(m_vertices.GetCapacity());
  if(n > 0)
  {
    for(int i = 0; i < n; ++i)
    {
      float d(m_vertices.GetVector3(i).Dot());
      if(d > radius)
      {
        radius = d;
      }
    }
    radius = sqrtf(radius);
  }
  return radius;
}

//==============================================================================
void  BasicMesh::CalculateExtents(Vector3& minOut, Vector3& maxOut) const
{
  const int n(m_vertices.GetCapacity());
  if(n > 0)
  {
    minOut.x = minOut.y = minOut.z = std::numeric_limits<float>::max();
    maxOut.x = maxOut.y = maxOut.z = -std::numeric_limits<float>::max();
    for(int i = 0; i < n; ++i)
    {
      Vector3 v(m_vertices.GetVector3(i));
      if(v.x < minOut.x)
      {
        minOut.x = v.x;
      }

      if(v.y < minOut.y)
      {
        minOut.y = v.y;
      }

      if(v.z < minOut.z)
      {
        minOut.z = v.z;
      }

      if(v.x > maxOut.x)
      {
        maxOut.x = v.x;
      }

      if(v.y > maxOut.y)
      {
        maxOut.y = v.y;
      }

      if(v.z > maxOut.z)
      {
        maxOut.z = v.z;
      }
    }
  }
  else
  {
    minOut = maxOut = Vector3::Zero();
  }
}

//==============================================================================
void  BasicMesh::Render()
{
  if (m_vertices.GetCapacity() > 0)
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

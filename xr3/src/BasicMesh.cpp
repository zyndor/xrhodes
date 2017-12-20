//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/BasicMesh.hpp"
#include "XR/Renderer.hpp"

namespace XR {

//==============================================================================
BasicMeshCore::BasicMeshCore(size_t vertexSize, size_t numVertices)
: m_vertices(vertexSize, numVertices)
{}

//==============================================================================
BasicMeshCore::~BasicMeshCore()
{
  Gfx::Destroy(m_vbo);
}

//==============================================================================
void  BasicMeshCore::SetMaterial(Material::Ptr const& material)
{
  m_material = material;
}

} // XR

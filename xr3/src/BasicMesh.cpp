//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/BasicMesh.hpp"

namespace XR {

//==============================================================================
BasicMeshCore::BasicMeshCore(uint32_t vertexSize, uint32_t numVertices)
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

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/MeshRenderer.hpp"
#include "XR/Renderer.hpp"

namespace XR
{

//==============================================================================
MeshRenderer::MeshRenderer()
: BaseType(),
  m_pMesh(0)
{}

//==============================================================================
MeshRenderer::~MeshRenderer()
{}

//==============================================================================
MeshRenderer*  MeshRenderer::Clone() const
{
  MeshRenderer*  pClone(new MeshRenderer());
  pClone->SetMesh(m_pMesh);
  return  pClone;
}

//==============================================================================
void  MeshRenderer::SetMesh(IndexMesh<VertexFormat>* pMesh)
{
  m_pMesh = pMesh;
}

//==============================================================================
void  MeshRenderer::Render()
{
  XR_ASSERT(MeshRenderer, GetOwner() != 0);
  XR_ASSERT(MeshRenderer, m_pMesh != 0);
  // TODO: probably introduce interim RendererComponent with model[view] matrix
  // stack update, rather than regenerating the world transform from scratch.
  Renderer::SetModelMatrix(GetOwner()->GetWorldTransform());
  m_pMesh->GetMaterial()->Apply();
  m_pMesh->RenderOnly(PrimType::TRI_LIST);
}

} // XR

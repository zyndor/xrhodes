//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include "MeshRenderer.hpp"

namespace XR
{

//==============================================================================
MeshRenderer::MeshRenderer ()
: Entity::ComponentT<MeshRenderer>(),
  m_pMesh(0)
{}

//==============================================================================
MeshRenderer::~MeshRenderer ()
{}

//==============================================================================
MeshRenderer*  MeshRenderer::Clone() const
{
  MeshRenderer*  pClone(new MeshRenderer());
  pClone->SetMesh(m_pMesh);
  return  pClone;
}

//==============================================================================
void  MeshRenderer::SetMesh(BasicMesh* pMesh)
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
  m_pMesh->Render();
}

} // XR

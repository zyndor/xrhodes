//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/MeshRenderer.hpp"
#include "xr/Mesh.hpp"
#include "xr/Transforms.hpp"

namespace xr
{

//==============================================================================
MeshRenderer::MeshRenderer()
: BaseType(),
  m_mesh(nullptr)
{}

//==============================================================================
MeshRenderer::~MeshRenderer()
{}

//==============================================================================
MeshRenderer*  MeshRenderer::Clone() const
{
  MeshRenderer*  pClone(new MeshRenderer());
  pClone->SetMesh(m_mesh);
  return  pClone;
}

//==============================================================================
void  MeshRenderer::SetMesh(Mesh* mesh)
{
  m_mesh = mesh;
}

//==============================================================================
void  MeshRenderer::Render()
{
  XR_ASSERT(MeshRenderer, GetOwner() != nullptr);
  XR_ASSERT(MeshRenderer, m_mesh != nullptr);
  // TODO: probably introduce interim RendererComponent with model[view] matrix
  // stack update, rather than regenerating the world transform from scratch.
  XR_TRANSFORMS_SCOPED_MODEL(GetOwner()->GetLocalTransform());
  material->Apply();
  m_mesh->Render(Primitive::TriList);
}

}

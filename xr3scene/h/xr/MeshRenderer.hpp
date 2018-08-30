#ifndef XR_MESHRENDERER_HPP
#define XR_MESHRENDERER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Entity.hpp"
#include "Component.hpp"
#include "xr/Material.hpp"

namespace xr
{

class Mesh;

//==============================================================================
class MeshRenderer:  public ComponentT<MeshRenderer>
{
public:
  // data
  Material::Ptr material;

  // structors
  MeshRenderer();
  ~MeshRenderer();

  // general
  virtual MeshRenderer*  Clone() const;

  Mesh* GetMesh() const;  // no ownership transfer
  void  SetMesh(Mesh* pMesh);  // no ownership transfer

  void  Render();

protected:
  // data
  Mesh*  m_mesh;  // no ownership
};

//==============================================================================
// implementation
//==============================================================================
inline
Mesh*  MeshRenderer::GetMesh() const
{
  return  m_mesh;
}

}

#endif // XR_MESHRENDERER_HPP

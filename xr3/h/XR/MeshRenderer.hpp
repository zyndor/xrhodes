//
// Nuclear Heart Games
// XRhodes
//
// MeshRenderer.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    16/04/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#ifndef XR_MESHRENDERER_HPP
#define XR_MESHRENDERER_HPP

#include "Entity.hpp"
#include "BasicMesh.hpp"

namespace XR
{

//==============================================================================
class MeshRenderer:  public Entity::ComponentT<MeshRenderer>
{
public:
  // structors
  MeshRenderer();
  ~MeshRenderer();
  
  // general
  virtual MeshRenderer*  Clone() const;
  
  BasicMesh*  GetMesh() const;  // no ownership transfer
  void        SetMesh(BasicMesh* pMesh);  // no ownership transfer
  
  void        Render();
  
protected:
  // data
  BasicMesh*  m_pMesh;  // no ownership
};

//==============================================================================
// implementation
//==============================================================================
inline
BasicMesh*  MeshRenderer::GetMesh() const
{
  return  m_pMesh;
}

} // XR

#endif // XR_MESHRENDERER_HPP

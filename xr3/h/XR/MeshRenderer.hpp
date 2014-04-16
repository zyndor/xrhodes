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
class MeshRenderer:  public Entity::Component
{
  XR_ENTITY_COMPONENT_GETTYPEID_DECL(MeshRenderer)
  
public:
  // types
  // static
  // data
  // structors
  MeshRenderer();
  ~MeshRenderer();
  
  // general
  virtual Component*  Clone() const;
  
  BasicMesh*  GetMesh() const;
  void        SetMesh(BasicMesh* pMesh);
  
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

//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_RENDERINGRESOURCE_HPP
#define XR_RENDERINGRESOURCE_HPP

#include "xrgl.hpp"

namespace XR
{

//=============================================================================
// An OpenGL rendering resource with an OpenGL handle.
class RenderingResource
{
protected:
  explicit RenderingResource(GLuint name)
  : m_name(name)
  {}

  RenderingResource(RenderingResource const&) = delete;

protected:
  GLuint  m_name;
};

} // XR

#endif XR_RENDERINGRESOURCE_HPP
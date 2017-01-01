//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_SURFACE_HPP
#define XR_SURFACE_HPP

#include "Texture.hpp"

namespace XR
{

class Surface:  protected CrossObject
{
  CROSS_OBJECT_DEF

public:
  // static
  void  MakeDefaultCurrent();

  // structors
  Surface();
  ~Surface();

  // general
  Texture GetTexture() const;

  void    MakeCurrent();
};

} // XR

#endif  //XR_SURFACE_HPP
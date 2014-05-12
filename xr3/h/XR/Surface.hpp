//
// Nuclear Heart Games
// XRhodes
//
// Surface.hpp
// @author  Gyorgy Straub
// @date    08/10/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
#if !defined XR_SURFACE_HPP
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
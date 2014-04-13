//
// Nuclear Heart Studios
// XRhodes
//
// Texture.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date  2013/09/11
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_TEXTURE_HPP
#define XR_TEXTURE_HPP

#include "CrossObject.hpp"
#include "ResManager.hpp"
#include "Hash.hpp"

namespace XR
{

//==============================================================================
class Texture: protected CrossObject
{
  XR_CROSS_OBJECT_DECL(Texture)
  XR_MANAGED_DECL(Texture)

public:
  // general
  int     GetWidth() const;
  int     GetHeight() const;
  int     GetPitch() const;
  
  bool    HasAlpha() const;
  
  bool    Load(const char* pName);

  uint32  GetFlags() const; // know your platform / implementation.
  
  bool    GetClamping() const;  // clamping [on] or wrapping (off)
  bool    GetFiltering() const;  // linear [on] or nearest (off)
  bool    GetMipMapping() const;
  bool    GetModifiable() const;  // pixel data is kept [on] or discarded.

  void    SetClamping(bool state);  // clamping [on] or wrapping (off)
  void    SetFiltering(bool state);  // linear [on] or nearest (off)
  void    SetMipMapping(bool state);
  void    SetModifiable(bool state);  // pixel data is kept [on] or discarded.

  void    Upload();
};

} // XR

#endif // XR_TEXTURE_HPP
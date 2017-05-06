//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_TEXTURE_HPP
#define XR_TEXTURE_HPP

#include "Image.hpp"
#include "ResManager.hpp"
#include <XR/Hash.hpp>

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
  void    CopyImage(const Image& img);

  uint32_t  GetFlags() const; // know your platform / implementation.
  
  bool    GetClamping() const;  // clamping [on] or wrapping (off)
  bool    GetFiltering() const;  // linear [on] or nearest (off)
  bool    GetMipMapping() const;
  bool    GetModifiable() const;  // pixel data is kept [on] or discarded.

  void    SetClamping(bool state);  // clamping [on] or wrapping (off)
  void    SetFiltering(bool state);  // linear [on] or nearest (off)
  void    SetMipMapping(bool state);
  void    SetModifiable(bool state);  // pixel data is kept [on] or discarded.

  void    Upload();

  uint32_t GetOpenGLHandle() const;
};

} // XR

#endif // XR_TEXTURE_HPP
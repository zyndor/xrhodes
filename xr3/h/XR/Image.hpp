//
// Nuclear Heart Games
// XRhodes
//
// Image.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    28/05/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#ifndef XR_IMAGE_HPP
#define XR_IMAGE_HPP

#include "types.hpp"
#include "CrossObject.hpp"

namespace XR
{

//==============================================================================
class Image:  protected CrossObject
{
  XR_NONCOPY_DECL(Image)
  XR_CROSS_OBJECT_USING()
  
public:
  // structors
  Image();
  ~Image();
  
  // general
  ///@brief Loads image from file named @a pFilename.
  ///@return  Result of operation.
  bool    Load(const char* pFilename);
  
  ///@brief Copies @A img, overwriting this image.
  void    Copy(const Image& img);
  
  ///@return  The width of the image in pixels.
  int     GetWidth() const;

  ///@return  The height of the image in pixels.
  int     GetHeight() const;

  ///@return  The pitch of the image in pixels.
  int     GetPitch() const;
  
  ///@return  The native format flag. Know your back-end.
  ///@TODO  Some form of centralised mapping of formats.
  uint32  GetFormatNative() const;

  ///@return  Number of bytes per pixel.
  int     GetBytesPerPixel() const;

  ///@return  Pointer to native pixel data.
  void*   GetPixelData() const;

  ///@return Number of colours in palette (0 if non-palettized).
  int     GetPaletteSize() const;

  ///@return  Pointer to palette (0 if non-palettized).
  ///@note    You can safely expect it to be an array of GetPalettSize()
  /// elements. Of what, will depend on the back-end.
  void*   GetPaletteNative() const;
};

} // XR

#endif // XR_IMAGE_HPP

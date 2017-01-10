//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_IMAGE_HPP
#define XR_IMAGE_HPP

#include "fundamentals.hpp"
#include "CrossObject.hpp"
#include <cstdint>

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
  
  ///@brief Copies @a img, overwriting this image.
  void    Copy(const Image& img);
  
  ///@return  The width of the image in pixels.
  int     GetWidth() const;

  ///@return  The height of the image in pixels.
  int     GetHeight() const;

  ///@return  The pitch of the image in pixels.
  int     GetPitch() const;
  
  ///@return  The native format flag. Know your back-end.
  ///@TODO  Some form of centralised mapping of formats.
  uint32_t  GetFormatNative() const;

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

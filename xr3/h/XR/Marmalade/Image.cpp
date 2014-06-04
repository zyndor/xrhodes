//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include  <IwImage.h>
#include  "File.hpp"
#include  "Image.hpp"

namespace XR
{

#define TO_IMPL() static_cast<CIwImage*>(GetImpl())
#define TO_CONST_IMPL() static_cast<const CIwImage*>(GetImpl())

static void  DestroyImage(Image& img)
{
  delete static_cast<CIwImage*>(img.SwapImpl(0));
}

//==============================================================================
Image::Image ()
: CrossObject(0)
{}

//==============================================================================
Image::~Image ()
{
  DestroyImage(*this);
}

//==============================================================================
bool  Image::Load(const char* pFilename)
{
  XR_ASSERT(Image, pFilename != 0);
  DestroyImage(*this);
  if(File::CheckExists(pFilename))
  {
    CIwImage* pImg(new CIwImage());
    pImg->LoadFromFile(pFilename);
    SwapImpl(pImg);
  }
  return HasImpl();
}

//==============================================================================
void  Image::Copy(const Image& img)
{
  TO_IMPL()->operator=(*static_cast<const CIwImage*>(img.GetImpl()));
}

//==============================================================================
int Image::GetWidth() const
{
  return TO_CONST_IMPL()->GetWidth();
}

//==============================================================================
int Image::GetHeight() const
{
  return TO_CONST_IMPL()->GetHeight();
}

//==============================================================================
int Image::GetPitch() const
{
  return TO_CONST_IMPL()->GetPitch();
}

//==============================================================================
uint32  Image::GetFormatNative() const
{
  return TO_CONST_IMPL()->GetFormat();
}
 
//==============================================================================
int Image::GetBytesPerPixel() const
{
  return TO_CONST_IMPL()->GetByteDepth();
}

//==============================================================================
void* Image::GetPixelData() const
{
  return TO_CONST_IMPL()->GetTexels();
}

//==============================================================================
int Image::GetPaletteSize() const
{
  return TO_CONST_IMPL()->GetPaletteSize();
}

//==============================================================================
void* Image::GetPaletteNative() const
{
  return TO_CONST_IMPL()->GetPalette();
}

} // XR

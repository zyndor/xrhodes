//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include  <XR/Image.hpp>
#include  <SDL_image.h>

namespace XR
{

#define TO_IMPL() static_cast<SDL_Surface*>(GetImpl())
#define TO_CONST_IMPL() static_cast<const SDL_Surface*>(GetImpl())

static void DestroyImage(Image& img)
{
  SDL_FreeSurface((SDL_Surface*)img.SwapImpl(0));
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
  DestroyImage(*this);
  SwapImpl(IMG_Load(pFilename));
  return HasImpl();
}

//==============================================================================
void  Image::Copy(const Image& img)
{
  SDL_Surface*  pSurf(0);
  if(img.GetPixelData() != 0)
  {
    const SDL_Surface*  pOther(static_cast<const SDL_Surface*>(img.GetImpl()));
    pSurf = SDL_CreateRGBSurfaceFrom(pOther->pixels, pOther->w, pOther->h,
      pOther->format->BitsPerPixel, pOther->pitch, pOther->format->Rmask,
      pOther->format->Gmask, pOther->format->Bmask, pOther->format->Amask);
  }

  DestroyImage(*this);
  SwapImpl(pSurf);
}

//==============================================================================
int Image::GetWidth() const
{
  const SDL_Surface*  pSurf(TO_CONST_IMPL());
  return pSurf != 0 ? pSurf->w : 0;
}

//==============================================================================
int Image::GetHeight() const
{
  const SDL_Surface*  pSurf(TO_CONST_IMPL());
  return pSurf != 0 ? pSurf->h : 0;
}

//==============================================================================
int Image::GetPitch() const
{
  const SDL_Surface*  pSurf(TO_CONST_IMPL());
  return pSurf != 0 ? pSurf->pitch : 0;
}

//==============================================================================
uint32  Image::GetFormatNative() const
{
  const SDL_Surface*  pSurf(TO_CONST_IMPL());
  return pSurf != 0 ? pSurf->format->format : 0;
}

//==============================================================================
int Image::GetBytesPerPixel() const
{
  const SDL_Surface*  pSurf(TO_CONST_IMPL());
  return pSurf != 0 ? pSurf->format->BytesPerPixel : 0;
}

//==============================================================================
void* Image::GetPixelData() const
{
  const SDL_Surface*  pSurf(TO_CONST_IMPL());
  return pSurf != 0 ? pSurf->pixels : 0;
}

//==============================================================================
int Image::GetPaletteSize() const
{
  const SDL_Palette*  pPalette(TO_CONST_IMPL()->format->palette);
  return pPalette != 0 ? pPalette->ncolors : 0;
}
//==============================================================================
void*  Image::GetPaletteNative() const
{
  const SDL_Palette*  pPalette(TO_CONST_IMPL()->format->palette);
  return pPalette != 0 ? pPalette->colors : 0;
}

} // XR

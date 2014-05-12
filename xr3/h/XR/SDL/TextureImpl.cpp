#include <SDL2/SDL_image.h>
#include "TextureImpl.hpp"

namespace XR
{

//==============================================================================
static const int  kNumMipMapLevels = 4;

//==============================================================================
TextureImpl::TextureImpl()
: m_pSurf(0),
  m_hTexture(INVALID_HANDLE),
  m_width(0),
  m_height(0),
  m_pitch(0),
  m_flags(XR_MASK_ID(F_CLAMPING))
{
  XR_GL_CALL(glGenTextures(1, &m_hTexture));
  XR_GL_CALL(glBindTexture(GL_TEXTURE_2D, m_hTexture));
}

//==============================================================================
TextureImpl::~TextureImpl()
{
  XR_GL_CALL(glDeleteTextures(1, &m_hTexture));

  FreeSurface();
}

//==============================================================================
bool  TextureImpl::LoadFromFile(const char* pFileName)
{
  FreeSurface();
  m_pSurf = IMG_Load(pFileName);

  bool  result(m_pSurf != 0);
  if (result)
  {
    m_width = m_pSurf->w;
    m_height = m_pSurf->h;
    m_pitch = m_pSurf->pitch;
  }

  return result;
}

//===========-===================================================================
uint32  TextureImpl::GetFlags() const
{
  return m_flags;
}

//==============================================================================
bool  TextureImpl::GetClamping() const
{
  return XR_MASK_HAS_ID(m_flags, F_CLAMPING);
}

//==============================================================================
bool  TextureImpl::GetFiltering() const
{
  return XR_MASK_HAS_ID(m_flags, F_FILTERING);
}

//==============================================================================
bool  TextureImpl::GetMipMapping() const
{
  return XR_MASK_HAS_ID(m_flags, F_MIPMAPPING);
}

//==============================================================================
bool  TextureImpl::GetModifiable() const
{
  return XR_MASK_HAS_ID(m_flags, F_MODIFIABLE);
}

//==============================================================================
void  TextureImpl::SetClamping(bool state)
{
  m_flags = state ? (m_flags | XR_MASK_ID(F_CLAMPING)) :
    (m_flags & ~XR_MASK_ID(F_CLAMPING));
}

//==============================================================================
void  TextureImpl::SetFiltering(bool state)
{
  m_flags = state ? (m_flags | XR_MASK_ID(F_FILTERING)) :
    (m_flags & ~XR_MASK_ID(F_FILTERING));
}

//==============================================================================
void  TextureImpl::SetMipMapping(bool state)
{
  m_flags = state ? (m_flags | XR_MASK_ID(F_MIPMAPPING)) :
    (m_flags & ~XR_MASK_ID(F_MIPMAPPING));
}

//==============================================================================
void  TextureImpl::SetModifiable(bool state)
{
  m_flags = state ? (m_flags | XR_MASK_ID(F_MODIFIABLE)) :
    (m_flags & ~XR_MASK_ID(F_MODIFIABLE));
}

//==============================================================================
void  TextureImpl::Upload()
{
  XR_ASSERT(TextureImpl, m_pSurf != 0);

  XR_GL_CALL(glBindTexture(GL_TEXTURE_2D, m_hTexture));

  GLenum  clampGl(GetClamping() ? GL_CLAMP_TO_EDGE : GL_REPEAT);
  XR_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampGl));
  XR_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampGl));
  
  GLenum  filterGl(GetFiltering() ? GL_LINEAR : GL_NEAREST);
  XR_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterGl));
  XR_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterGl));
  
  GLenum  pixelFormat(GL_INVALID_ENUM);
  int     bytesPerPixel(m_pSurf->format->BytesPerPixel);
  switch (bytesPerPixel)
  {
  case  1:
    pixelFormat = GL_LUMINANCE;
    break;
  case  3:
    pixelFormat = GL_RGB;
    break;
  case  4:
    pixelFormat = GL_RGBA;
    break;
  }

  int mipMapLevels(GetMipMapping() ? kNumMipMapLevels : 1);
  for (int i = 0; i < mipMapLevels; ++i)
  {
    XR_GL_CALL(glTexImage2D(GL_TEXTURE_2D, i, bytesPerPixel, m_width, m_height,
      0, pixelFormat, GL_UNSIGNED_BYTE, m_pSurf->pixels));
  }

  if (!GetModifiable())
  {
    FreeSurface();
  }
}

//==============================================================================
void  TextureImpl::FreeSurface()
{
  SDL_FreeSurface(m_pSurf);
  m_pSurf = 0;
}

} // XR

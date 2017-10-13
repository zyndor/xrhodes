//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "TextureImpl.hpp"
#include "XR/utils.hpp"
#include <SDL_image.h>
#include <cmath>

namespace XR
{

//==============================================================================
GLuint TextureImpl::New()
{
  GLuint name;
  XR_GL_CALL(glGenTextures(1, &name));
  return name;
}

//==============================================================================
TextureImpl::TextureImpl()
: RenderingResource(New()),
  m_pSurface(nullptr),
  m_width(0),
  m_height(0),
  m_flags(XR_MASK_ID(decltype(m_flags), F_CLAMPING))
{
  XR_GL_CALL(glBindTexture(GL_TEXTURE_2D, m_name));
}

//==============================================================================
TextureImpl::~TextureImpl()
{
  XR_GL_CALL(glDeleteTextures(1, &m_name));

  FreeSurface();
}

//==============================================================================
bool  TextureImpl::LoadFromFile(const char* pFileName)
{
  FreeSurface();
  m_pSurface = IMG_Load(pFileName);

  bool  result(m_pSurface != nullptr);
  if (result)
  {
    m_width = m_pSurface->w;
    m_height = m_pSurface->h;
  }

  return result;
}

//==============================================================================
void  TextureImpl::CopyImage(const Image& img)
{
  FreeSurface();
  
  Image temp;
  temp.Copy(img);
  
  m_pSurface = static_cast<SDL_Surface*>(temp.SwapImpl(nullptr));
}

//==============================================================================
uint32_t  TextureImpl::GetFlags() const
{
  return m_flags;
}

//==============================================================================
bool  TextureImpl::GetClamping() const
{
  return (m_flags & XR_MASK_ID(decltype(m_flags), F_CLAMPING)) > 0;
}

//==============================================================================
bool  TextureImpl::GetFiltering() const
{
  return (m_flags & XR_MASK_ID(decltype(m_flags), F_FILTERING)) > 0;
}

//==============================================================================
bool  TextureImpl::GetMipMapping() const
{
  return (m_flags & XR_MASK_ID(decltype(m_flags), F_MIPMAPPING)) > 0;
}

//==============================================================================
bool  TextureImpl::GetModifiable() const
{
  return (m_flags & XR_MASK_ID(decltype(m_flags), F_MODIFIABLE)) > 0;
}

//==============================================================================
void  TextureImpl::SetClamping(bool state)
{
  m_flags = state ? (m_flags | XR_MASK_ID(decltype(m_flags), F_CLAMPING)) :
    (m_flags & ~XR_MASK_ID(decltype(m_flags), F_CLAMPING));
}

//==============================================================================
void  TextureImpl::SetFiltering(bool state)
{
  m_flags = state ? (m_flags | XR_MASK_ID(decltype(m_flags), F_FILTERING)) :
    (m_flags & ~XR_MASK_ID(decltype(m_flags), F_FILTERING));
}

//==============================================================================
void  TextureImpl::SetMipMapping(bool state)
{
  m_flags = state ? (m_flags | XR_MASK_ID(decltype(m_flags), F_MIPMAPPING)) :
    (m_flags & ~XR_MASK_ID(decltype(m_flags), F_MIPMAPPING));
}

//==============================================================================
void  TextureImpl::SetModifiable(bool state)
{
  m_flags = state ? (m_flags | XR_MASK_ID(decltype(m_flags), F_MODIFIABLE)) :
    (m_flags & ~XR_MASK_ID(decltype(m_flags), F_MODIFIABLE));
}

//==============================================================================
void  TextureImpl::Upload()
{
  XR_ASSERT(TextureImpl, m_pSurface != nullptr);

  XR_GL_CALL(glBindTexture(GL_TEXTURE_2D, m_name));

  GLenum  clampGl(GetClamping() ? GL_CLAMP_TO_EDGE : GL_REPEAT);
  XR_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampGl));
  XR_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampGl));
  
  GLenum  filterGl(GetFiltering() ? GL_LINEAR : GL_NEAREST);
  XR_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterGl));
  XR_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterGl));
  
  GLenum  pixelFormat(GL_INVALID_ENUM);
  int     bytesPerPixel(m_pSurface->format->BytesPerPixel);
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

  int mipMapLevels(GetMipMapping() ?
    int(std::log2(std::min(m_pSurface->w, m_pSurface->h))): 1);
  for (int i = 0; i < mipMapLevels; ++i)
  {
    XR_GL_CALL(glTexImage2D(GL_TEXTURE_2D, i, bytesPerPixel, m_width, m_height,
      0, pixelFormat, GL_UNSIGNED_BYTE, m_pSurface->pixels));
  }

  if (!GetModifiable())
  {
    FreeSurface();
  }
}

//==============================================================================
void  TextureImpl::FreeSurface()
{
  SDL_FreeSurface(m_pSurface);
  m_pSurface = nullptr;
}

//==============================================================================
uint32_t TextureImpl::GetHandle() const
{
  return m_name;
}

} // XR

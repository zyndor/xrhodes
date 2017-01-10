//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_TEXTUREIMPL_HPP
#define XR_TEXTUREIMPL_HPP

#include "xrgl.hpp"
#include <XR/fundamentals.hpp>
#include <XR/Texture.hpp>

namespace XR
{

//==============================================================================
class TextureImpl
{
  XR_NONCOPY_DECL(TextureImpl)

public:
  // types
  enum
  {
    INVALID_HANDLE = -1
  };
  
  enum  Flags
  {
    F_CLAMPING,
    F_FILTERING,
    F_MIPMAPPING,
    F_MODIFIABLE
  };
  
  // structors
  TextureImpl();
  ~TextureImpl();

  // general
  int32_t GetWidth() const;
  int32_t GetHeight() const;
  int32_t GetPitch() const;

  bool    LoadFromFile(const char* pFileName);
  void    CopyImage(const Image& img);

  void    Bind(GLenum target);
  
  uint32_t  GetFlags() const;
  
  bool    GetClamping() const;
  bool    GetFiltering() const;
  bool    GetMipMapping() const;
  bool    GetModifiable() const;
  
  void    SetClamping(bool state);
  void    SetFiltering(bool state);
  void    SetMipMapping(bool state);
  void    SetModifiable(bool state);

  void    Upload();
  
  void    FreeSurface();

  uint32_t GetHandle() const; // no ownership transfer

protected:
  // data
  SDL_Surface*  m_pSurface;
  
  GLuint        m_hTexture;
  int32_t       m_width;
  int32_t       m_height;
  
  uint32_t      m_flags;
};

//==============================================================================
// implementation
//==============================================================================
inline
int32_t TextureImpl::GetWidth() const
{
  return m_width;
}

//==============================================================================
inline
int32_t TextureImpl::GetHeight() const
{
  return m_height;
}

//==============================================================================
inline
int32_t TextureImpl::GetPitch() const
{
  XR_ASSERT(TextureImpl, m_pSurface != 0);
  return m_pSurface->pitch;
}

//==============================================================================
inline
void  TextureImpl::Bind(GLenum target)
{
  XR_ASSERT(TextureImpl, m_hTexture != INVALID_HANDLE);
  XR_GL_CALL(glBindTexture(target, m_hTexture));
}

} // XR

#endif
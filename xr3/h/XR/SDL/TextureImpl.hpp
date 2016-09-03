//
// Nuclear Heart Games
// XRhodes
//
// TextureImpl`.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    24/11/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_TEXTUREIMPL_HPP
#define XR_TEXTUREIMPL_HPP

#include "xrgl.hpp"
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
  int32   GetWidth() const;
  int32   GetHeight() const;
  int32   GetPitch() const;

  bool    LoadFromFile(const char* pFileName);
  void    CopyImage(const Image& img);

  void    Bind(GLenum target);
  
  uint32  GetFlags() const;
  
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

protected:
  // data
  SDL_Surface*  m_pSurface;
  
  GLuint        m_hTexture;
  int32         m_width;
  int32         m_height;
  
  uint32        m_flags;
};

//==============================================================================
// implementation
//==============================================================================
inline
int32 TextureImpl::GetWidth() const
{
  return m_width;
}

//==============================================================================
inline
int32 TextureImpl::GetHeight() const
{
  return m_height;
}

//==============================================================================
inline
int32 TextureImpl::GetPitch() const
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
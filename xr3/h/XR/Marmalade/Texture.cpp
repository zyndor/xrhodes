#include "Texture.hpp"
#include <IwTexture.h>
#include "File.hpp"

//==============================================================================
#define TO_IMPL()  (static_cast<CIwTexture*>(GetImpl()))
#define TO_CONST_IMPL()  (static_cast<const CIwTexture*>(GetImpl()))

namespace XR
{

//==============================================================================
XR_CROSS_OBJECT_DEF(Texture, CIwTexture)
XR_CROSS_MANAGED_DEF(Texture, CIwTexture)

//==============================================================================
int Texture::GetWidth() const
{
  return TO_CONST_IMPL()->GetWidth();
}

//==============================================================================
int Texture::GetHeight() const
{
  return TO_CONST_IMPL()->GetHeight();
}

//==============================================================================
int Texture::GetPitch() const
{
  return TO_CONST_IMPL()->GetPitch();
}

//==============================================================================
bool  Texture::HasAlpha() const
{
  return TO_CONST_IMPL()->HasAlpha() != 0;
}

//==============================================================================
bool  Texture::Load(const char* pName)
{
  XR_ASSERT(Texture, pName != 0);
  TO_IMPL()->LoadFromFile(pName);
  return !TO_CONST_IMPL()->IsCountZero();
}

//==============================================================================
void  Texture::CopyImage(const Image& img)
{
  XR_ASSERT(Texture, img.HasImpl());
  CIwImage* pImg(static_cast<CIwImage*>(const_cast<void*>(img.GetImpl())));
  TO_IMPL()->CopyFromImage(pImg);
}

//==============================================================================
uint32  Texture::GetFlags() const
{
  return TO_CONST_IMPL()->GetFlags();
}

//==============================================================================
bool  Texture::GetClamping() const
{
  return TO_CONST_IMPL()->GetClamping();
}

//==============================================================================
bool  Texture::GetFiltering() const
{
  return TO_CONST_IMPL()->GetFiltering();
}

//==============================================================================
bool  Texture::GetMipMapping() const
{
  return TO_CONST_IMPL()->GetMipMapping();
}

//==============================================================================
bool  Texture::GetModifiable() const
{
  return TO_CONST_IMPL()->GetModifiable();
}

//==============================================================================
void  Texture::SetClamping(bool state)
{
  TO_IMPL()->SetClamping(state);
}

//==============================================================================
void  Texture::SetFiltering(bool state)
{
  TO_IMPL()->SetFiltering(state);
}

//==============================================================================
void  Texture::SetMipMapping(bool state)
{
  TO_IMPL()->SetMipMapping(state);
}

//==============================================================================
void  Texture::SetModifiable(bool state)
{
  TO_IMPL()->SetModifiable(state);
}

//==============================================================================
void  Texture::Upload()
{
  TO_IMPL()->Upload();
}

//==============================================================================
uint32_t Texture::GetOpenGLHandle() const
{
  return TO_CONST_IMPL()->m_HWID;
}

} // XR
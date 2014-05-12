#include "Texture.hpp"
#include "TextureImpl.hpp"

#define TO_IMPL()  (static_cast<TextureImpl*>(GetImpl()))
#define TO_CONST_IMPL()  (static_cast<const TextureImpl*>(GetImpl()))

namespace XR
{

//==============================================================================
XR_CROSS_OBJECT_DEF(Texture, TextureImpl)
XR_CROSS_MANAGED_DEF(Texture, TextureImpl)

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
bool  Texture::Load(const char* pName)
{
  return TO_IMPL()->LoadFromFile(pName);
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
  TO_IMPL()->SetMipMapping(state);
}

//==============================================================================
void  Texture::Upload()
{
  TO_IMPL()->Upload();
}

} // XR
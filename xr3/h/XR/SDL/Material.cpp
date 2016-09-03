//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include "MaterialImpl.hpp"
#include <XR/Material.hpp>
#include <algorithm>

#define TO_IMPL()  (static_cast<MaterialImpl*>(GetImpl()))
#define TO_CONST_IMPL()  (static_cast<const MaterialImpl*>(GetImpl()))

namespace XR
{

//==============================================================================
XR_CROSS_OBJECT_DEF(Material, MaterialImpl)
XR_CROSS_MANAGED_DEF(Material, MaterialImpl)

//==============================================================================
Material::AlphaMode Material::GetAlphaMode() const
{
  return static_cast<AlphaMode>(TO_CONST_IMPL()->alphaMode);
}

//==============================================================================
Material::AlphaTestMode Material::GetAlphaTestMode() const
{
  return static_cast<AlphaTestMode>(TO_CONST_IMPL()->alphaTestMode + 1 - GL_NEVER);
}

//==============================================================================
float Material::GetAlphaTestRefValue() const
{
  return TO_CONST_IMPL()->alphaTestRefValue;
}

//==============================================================================
Material::BlendMode Material::GetBlendMode() const
{
  return static_cast<BlendMode>(TO_CONST_IMPL()->blendMode);
}

//==============================================================================
static const GLenum karCullModeMappings[] =
{
  GL_BACK,
  GL_FRONT,
  GL_NONE
};

Material::CullMode Material::GetCullMode() const
{
  GLenum  cullMode(TO_CONST_IMPL()->cullMode);
  return static_cast<CullMode>(std::find(karCullModeMappings,
    karCullModeMappings + 3, cullMode) - karCullModeMappings);
}

//==============================================================================
bool  Material::GetDepthWriteEnabled() const
{
  return TO_CONST_IMPL()->depthWriteEnabled;
}

//==============================================================================
void Material::SetAlphaMode( AlphaMode am )
{
  TO_IMPL()->alphaMode = am;
}

//==============================================================================
void Material::SetAlphaTestMode( AlphaTestMode atm )
{
  TO_IMPL()->alphaTestMode = atm + GL_NEVER - 1;
}

//==============================================================================
void Material::SetAlphaTestRefValue(float f)
{
  TO_IMPL()->alphaTestRefValue = f;
}

//==============================================================================
void Material::SetBlendMode( BlendMode bm )
{
  TO_IMPL()->blendMode = bm;
}

//==============================================================================
void Material::SetCullMode( CullMode cm )
{
  TO_IMPL()->cullMode = karCullModeMappings[cm];  
}

//==============================================================================
void Material::SetDepthWriteEnabled(bool dw )
{
  TO_IMPL()->depthWriteEnabled = dw;
}

//==============================================================================
Color Material::GetAmbientColor() const
{
  return TO_CONST_IMPL()->colAmbient;
}

//==============================================================================
Color Material::GetDiffuseColor() const
{
  return TO_CONST_IMPL()->colDiffuse;
}

//==============================================================================
Color Material::GetSpecularColor() const
{
  return TO_CONST_IMPL()->colSpecular;
}

//==============================================================================
Color Material::GetEmissiveColor() const
{
  return TO_CONST_IMPL()->colEmissive;
}

//==============================================================================
void  Material::SetAmbientColor(const Color& col)
{
  TO_IMPL()->colAmbient = col;
}

//=========================================l=====================================
void  Material::SetDiffuseColor(const Color& col)
{
  TO_IMPL()->colDiffuse = col;
}

//==============================================================================
void  Material::SetSpecularColor(const Color& col)
{
  TO_IMPL()->colSpecular = col;
}

//==============================================================================
void  Material::SetEmissiveColor(const Color& col)
{
  TO_IMPL()->colEmissive = col;
}

//==============================================================================
Texture Material::GetTexture(int id) const
{
  return Texture(TO_CONST_IMPL()->arpTextures[id]);
}

//==============================================================================
void  Material::SetTexture(int id, Texture& t)
{
  TO_IMPL()->arpTextures[id] = static_cast<TextureImpl*>(t.GetImpl());
}

//==============================================================================
void  Material::Copy(const Material& rhs)
{
  XR_ASSERT(Material, m_pImpl != 0);
  XR_ASSERT(Material, rhs.m_pImpl != 0);
  TO_IMPL()->Copy(*static_cast<const MaterialImpl*>(rhs.m_pImpl));
}

} // XR
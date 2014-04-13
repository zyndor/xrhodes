#include "Material.hpp"
#include <IwMaterial.h>

//==============================================================================
#define TO_IMPL()  (static_cast<CIwMaterial*>(GetImpl()))
#define TO_CONST_IMPL()  (static_cast<const CIwMaterial*>(GetImpl()))

namespace XR
{

//==============================================================================
XR_CROSS_OBJECT_DEF(Material, CIwMaterial)
XR_CROSS_MANAGED_DEF(Material, CIwMaterial)

//==============================================================================
static const Material::AlphaMode  karAlphaModeMappings[] =
{
  Material::ALPHA_NONE,
  Material::ALPHA_HALF,
  Material::ALPHA_ADD,
  Material::ALPHA_SUB,
  Material::ALPHA_BLEND,
  Material::ALPHA_DEFAULT
};

Material::AlphaMode Material::GetAlphaMode() const
{
  return karAlphaModeMappings[TO_CONST_IMPL()->GetAlphaMode()];
}

//==============================================================================
static const Material::AlphaTestMode  karAlphaTestModeMappings[] =
{
  Material::ALPHATEST_DISABLED,
  Material::ALPHATEST_NEVER,
  Material::ALPHATEST_LESS,
  Material::ALPHATEST_EQUAL,
  Material::ALPHATEST_LEQUAL,
  Material::ALPHATEST_GREATER,
  Material::ALPHATEST_NOTEQUAL,
  Material::ALPHATEST_GEQUAL,
};

Material::AlphaTestMode Material::GetAlphaTestMode() const
{
  return karAlphaTestModeMappings[TO_CONST_IMPL()->GetAlphaTestMode()];
}

//==============================================================================
float Material::GetAlphaTestRefValue() const
{
  return TO_CONST_IMPL()->GetAlphaTestRefVal() * XR_ONE_OVER_0XFF;
}

//==============================================================================
static const Material::BlendMode  karBlendModeMappings[] = 
{
  Material::BLEND_MODULATE, //!< multiply;
  Material::BLEND_DECAL,    //!< decal;
  Material::BLEND_ADD,      //!< additive;
  Material::BLEND_REPLACE,  //!< replace;
  Material::BLEND_BLEND,    //!< blend (strange inverting behaviour);
  Material::BLEND_MODULATE_2X, //!< blend 2x
  Material::BLEND_MODULATE_4X, //!< blend 4x
};

Material::BlendMode Material::GetBlendMode() const
{
  return karBlendModeMappings[TO_CONST_IMPL()->GetBlendMode()];
}

//==============================================================================
static const Material::CullMode karCullModeMappings[] =
{
  Material::CULL_FRONT,
  Material::CULL_BACK,
  Material::CULL_NONE
};

Material::CullMode Material::GetCullMode() const
{
  return karCullModeMappings[const_cast<CIwMaterial*>(TO_CONST_IMPL())->GetCullMode()];
}

//==============================================================================
bool  Material::GetDepthWriteEnabled() const
{
  return TO_CONST_IMPL()->GetDepthWriteMode() == CIwMaterial::DEPTH_WRITE_NORMAL;
}

//==============================================================================
static const CIwMaterial::AlphaMode karAlphaModeRemappings[] =
{
  CIwMaterial::ALPHA_NONE,
  CIwMaterial::ALPHA_HALF,
  CIwMaterial::ALPHA_ADD,
  CIwMaterial::ALPHA_SUB,
  CIwMaterial::ALPHA_BLEND,
  CIwMaterial::ALPHA_DEFAULT,
};

void Material::SetAlphaMode( AlphaMode am )
{
  TO_IMPL()->SetAlphaMode(karAlphaModeRemappings[am]);  
}

//==============================================================================
static const CIwMaterial::AlphaTestMode karAlphaTestModeRemappings[] =
{
  CIwMaterial::ALPHATEST_DISABLED,
  CIwMaterial::ALPHATEST_NEVER,
  CIwMaterial::ALPHATEST_LESS,
  CIwMaterial::ALPHATEST_EQUAL,
  CIwMaterial::ALPHATEST_LEQUAL,
  CIwMaterial::ALPHATEST_GREATER,
  CIwMaterial::ALPHATEST_NOTEQUAL,
  CIwMaterial::ALPHATEST_GEQUAL,
  CIwMaterial::ALPHATEST_ALWAYS,
};

void Material::SetAlphaTestMode( AlphaTestMode atm )
{
  TO_IMPL()->SetAlphaTestMode(karAlphaTestModeRemappings[atm]);  
}

//==============================================================================
void  Material::SetAlphaTestRefValue(float f)
{
  IwAssert(Material, f >= .0f);
  IwAssert(Material, f <= 1.0f);
  TO_IMPL()->SetAlphaTestRefVal((uint8)(f * 0xff));
}

//==============================================================================
static const CIwMaterial::BlendMode karBlendModeRemappings[] =
{
  CIwMaterial::BLEND_MODULATE, //!< multiply;
  CIwMaterial::BLEND_DECAL,    //!< decal;
  CIwMaterial::BLEND_ADD,      //!< additive;
  CIwMaterial::BLEND_REPLACE,  //!< replace;
  CIwMaterial::BLEND_BLEND,    //!< blend (strange inverting behaviour);
  CIwMaterial::BLEND_MODULATE_2X, //!< blend 2x
  CIwMaterial::BLEND_MODULATE_4X, //!< blend 4x
};

void Material::SetBlendMode( BlendMode bm )
{
  TO_IMPL()->SetBlendMode(karBlendModeRemappings[bm]);  
}

//==============================================================================
static const CIwMaterial::CullMode  karCullModeRemappings[] =
{
  CIwMaterial::CULL_BACK,
  CIwMaterial::CULL_FRONT,
  CIwMaterial::CULL_NONE
};

void Material::SetCullMode( CullMode cm )
{
  TO_IMPL()->SetCullMode(karCullModeRemappings[cm]);  
}

//==============================================================================
static const CIwMaterial::DepthWriteMode  karDepthWriteRemappings[] =
{
  CIwMaterial::DEPTH_WRITE_NORMAL,
  CIwMaterial::DEPTH_WRITE_DISABLED
};

//==============================================================================
void Material::SetDepthWriteEnabled( bool state)
{
  TO_IMPL()->SetDepthWriteMode(state ? CIwMaterial::DEPTH_WRITE_NORMAL :
    CIwMaterial::DEPTH_WRITE_DISABLED);  
}

//==============================================================================
void  Material::SetAmbientColor(const Color& c)
{
  TO_IMPL()->SetColAmbient(c.GetABGR());
}

//==============================================================================
Color  Material::GetAmbientColor() const
{
  return Color(static_cast<CIwMaterial*>(m_pImpl)->GetColAmbient().Get());
}

//==============================================================================
void  Material::SetTexture(int id, Texture& t)
{
  TO_IMPL()->SetTexture(static_cast<CIwTexture*>(t.GetImpl()), id);
}

//==============================================================================
Texture Material::GetTexture(int id) const
{
  return Texture(TO_CONST_IMPL()->GetTexture(id));
}

//==============================================================================
void  Material::Copy(const Material& rhs)
{
  XR_ASSERT(Material, m_pImpl != 0);
  XR_ASSERT(Material, rhs.m_pImpl != 0);
  TO_IMPL()->Copy(*static_cast<const CIwMaterial*>(rhs.m_pImpl));
}

} // XR
//
// Nuclear Heart Studios
// XRhodes
//
// Material.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date  2013/09/11
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_MATERIAL_HPP
#define XR_MATERIAL_HPP

#include "Color.hpp"
#include "Texture.hpp"

namespace XR
{

//==============================================================================
class Material: protected CrossObject
{
  XR_CROSS_OBJECT_DECL(Material)
  XR_MANAGED_DECL(Material)

public:
  // typedefs
  enum  AlphaMode
  {
    ALPHA_NONE,
    ALPHA_HALF,
    ALPHA_ADD,
    ALPHA_SUB,
    ALPHA_BLEND,
    ALPHA_DEFAULT
  };
  
  enum  AlphaTestMode
  {
    ALPHATEST_DISABLED,
    ALPHATEST_NEVER,
    ALPHATEST_LESS,
    ALPHATEST_EQUAL,
    ALPHATEST_LEQUAL,
    ALPHATEST_GREATER,
    ALPHATEST_NOTEQUAL,
    ALPHATEST_GEQUAL,
    ALPHATEST_ALWAYS,
  };
  
  enum BlendMode
  {
    BLEND_MODULATE, //!< multiply;
    BLEND_DECAL,    //!< decal;
    BLEND_ADD,      //!< additive;
    BLEND_REPLACE,  //!< replace;
    BLEND_BLEND,    //!< blend (strange inverting behaviour);
    BLEND_MODULATE_2X, //!< blend 2x
    BLEND_MODULATE_4X, //!< blend 4x
  };
  
  enum  CullMode
  {
    CULL_BACK,
    CULL_FRONT,
    CULL_NONE
  };

  // general
  AlphaMode     GetAlphaMode() const;
  AlphaTestMode GetAlphaTestMode() const;
  float         GetAlphaTestRefValue() const;
  BlendMode     GetBlendMode() const;
  CullMode      GetCullMode() const;
  bool          GetDepthWriteEnabled() const;
  
  void          SetAlphaMode(AlphaMode am);
  void          SetAlphaTestMode(AlphaTestMode atm);
  void          SetAlphaTestRefValue(float f);
  void          SetBlendMode(BlendMode bm);
  void          SetCullMode(CullMode cm);
  void          SetDepthWriteEnabled(bool state);

  void          SetAmbientColor(const Color& c);
  Color         GetAmbientColor() const;

  void          SetTexture(int id, Texture& t);
  Texture       GetTexture(int id) const;
  //void          SetShaderTechnique(Shader* pShader);

  void          Copy(const Material& rhs);
};

} // XR

#endif // XR_MATERIAL_HPP
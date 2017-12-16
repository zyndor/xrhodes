#ifndef XR_MATERIAL_HPP
#define XR_MATERIAL_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Color.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

namespace XR
{

//==============================================================================
class Material: public Asset
{
public:
  XR_ASSET_DECL(Material)

  // typedefs
  // TODO: remove or implement options properly.
  enum  AlphaMode
  {
    ALPHA_NONE,
    ALPHA_HALF,
    ALPHA_ADD,
    ALPHA_SUB,
    ALPHA_BLEND,
    ALPHA_DEFAULT
  };

  // TODO: remove and replace with asset dependency in Font and TexturePack.
  struct Manager
  {
    static Material* Get(char const* name, void* data) { return nullptr; }
  };
  using GetCallback = Material*(*)(char const* name, void* data);
  // end remove

  enum { kMaxTextureStages = 8 };  // TODO: expose in Gfx, remove from here.

  // general
  uint32_t  GetStateFlags() const;
  void      OverrideStateFlags(uint32_t clear, uint32_t set);

  void  SetTexture(int stage, Texture::Ptr const& texture);
  Texture::Ptr  GetTexture(int stage) const;

  void  SetShader(Shader::Ptr const& shader);
  Shader::Ptr GetShader();

  void Apply() const;

  // Inherited via Asset
  bool OnLoaded(Buffer buffer) override;

  void OnUnload() override;

private:
  uint32_t      m_stateFlags = 0;
  Texture::Ptr  m_textureStages[kMaxTextureStages];
  Shader::Ptr   m_shader;
};

} // XR

#endif // XR_MATERIAL_HPP

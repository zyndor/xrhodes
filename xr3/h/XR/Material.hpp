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
///@brief A material asset that can be applied to geometries for rendering,
/// creatable from a .mtl file, which is a XON document with support for the
/// following properties:
/// shader: the name of the file to load the shader from; required.
/// textures: the name of the files to load the various texture stages from;
///   optional. Currently up to 8 textures are supported which may be specified
///   either as a XON value (for a single texture), key-value pairs for texture
///   stage and path, i.e. 0: my/texture.png, 1: my/other_texture.png.
/// state: the rendering states to set; a combination of the following are
///   supported: depthTest, depthWrite, alphaBlend, cullBack, cullFront,
///     wireframe.
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

  enum { kMaxTextureStages = 8 };  // TODO: expose in Gfx, remove from here.

  // general
  ///@return The flags that this Material passes to XR::Gfx::SetState().
  uint32_t  GetStateFlags() const;

  ///@brief Unsets @a clear, sets @a set of the state flags.
  void      OverrideStateFlags(uint32_t clear, uint32_t set);

  ///@brief Sets @a texture for the given @a stage.
  ///@note A maximum of 8 textures per Material is supported.
  void  SetTexture(int stage, Texture::Ptr const& texture);

  ///@return The texture set for the given @a stage.
  Texture::Ptr  GetTexture(int stage) const;

  ///@brief Sets the @a Shader used by this Material.
  void  SetShader(Shader::Ptr const& shader);

  ///@return The Shader used by this Material.
  Shader::Ptr GetShader() const;

  ///@brief Sets the Material for the subsequent drawing commands,
  /// passing its state, texture and shader information to Gfx.
  void Apply() const;

private:
  // data
  uint32_t      m_stateFlags = 0;
  Texture::Ptr  m_textureStages[kMaxTextureStages];
  Shader::Ptr   m_shader;

  // internal
  bool OnLoaded(Buffer buffer) override;
  void OnUnload() override;
};

} // XR

#endif // XR_MATERIAL_HPP

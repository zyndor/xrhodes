#ifndef XR_TEXTUREPACK_HPP
#define XR_TEXTUREPACK_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <map>
#include "Sprite.hpp"
#include "Asset.hpp"

namespace XR
{

//==============================================================================
///@brief A texture atlas based on TexturePacker's Generic XML format, with a
/// simple addition: it supports a shaderPath attribute on the root element.
class TexturePack: public Asset
{
public:
  XR_ASSET_DECL(TexturePack)

  // types
  typedef std::map<uint32_t, Sprite>  SpriteMap;

  // static
  static Shader::Ptr* s_defaultShader;

  // general
  Material::Ptr     GetMaterial() const;

  size_t            CountSprites() const;

  Sprite*           Get(char const* name, bool allowMissing); // no ownership transfer
  Sprite*           Get(char const* name); // no ownership transfer
  Sprite*           Get(uint32_t hash); // no ownership transfer

  const Sprite*     Get(char const* name, bool allowMissing) const; // no ownership transfer
  const Sprite*     Get(char const* name) const;
  const Sprite*     Get(uint32_t hash) const;

  const SpriteMap&  GetSprites() const;

  void              ScaleSprites(float x);
  void              ScaleSprites(float x, float y);

  void              Clear();

  bool OnLoaded(Buffer buffer) override;
  void OnUnload() override;

protected:
  // data
  Material::Ptr m_material;
  SpriteMap  m_sprites;
};

//==============================================================================
// implementation
//==============================================================================
inline
Material::Ptr TexturePack::GetMaterial() const
{
  return m_material;
}

//==============================================================================
inline
size_t TexturePack::CountSprites() const
{
  return m_sprites.size();
}

//==============================================================================
inline
const TexturePack::SpriteMap& TexturePack::GetSprites() const
{
  return m_sprites;
}

//==============================================================================
inline
Sprite* TexturePack::Get( uint32_t hash )
{
  SpriteMap::iterator  iFind(m_sprites.find(hash));
  return iFind != m_sprites.end() ? &iFind->second : 0;
}

//==============================================================================
inline
const Sprite* TexturePack::Get( uint32_t hash ) const
{
  SpriteMap::const_iterator  iFind(m_sprites.find(hash));
  return iFind != m_sprites.end() ? &iFind->second : 0;
}

} // XR

#endif // XR_TEXTUREPACK_HPP

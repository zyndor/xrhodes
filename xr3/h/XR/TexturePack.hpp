//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_TEXTUREPACK_HPP
#define XR_TEXTUREPACK_HPP

#include <map>
#include "Sprite.hpp"

namespace XR
{

//==============================================================================
class TexturePack
{
public:
  // types
  typedef std::map<uint32_t, Sprite>  SpriteMap;

  // structors
  TexturePack();
  ~TexturePack();

  // general
  ///@deprecated Asset::Manager integration in progress.
  ///@brief Attempts to load a texture pack from TexturePacker's 'Generic XML'
  /// format. Requires a material in the same location and name (but with '.mtl'
  /// extension)as the texture in its definition.
  bool              Load(char const* name, Asset::FlagType flags = 0);

  Material::Ptr     GetMaterial() const;  // no ownership transfer

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

protected:
  // data
  Material::Ptr  m_material;  // no ownership
  SpriteMap  m_sprites;
};

//==============================================================================
// implementation
//==============================================================================
inline
Material::Ptr TexturePack::GetMaterial() const
{
  return m_material; // no ownership transfer
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
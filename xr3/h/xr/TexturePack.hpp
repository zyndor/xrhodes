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
#include "Material.hpp"
#include "Asset.hpp"
#include "IMaterialisable.hpp"

namespace xr
{

//==============================================================================
///@brief A texture atlas based on TexturePacker's Generic XML format.
class TexturePack: public Asset, public IMaterialisable
{
public:
  XR_ASSET_DECL(TexturePack)

  // types
  typedef std::map<uint32_t, Sprite>  SpriteMap;

  // general
  Texture::Ptr      GetTexture() const override;

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
  Texture::Ptr m_texture;
  SpriteMap  m_sprites;
};

//==============================================================================
// implementation
//==============================================================================
inline
Texture::Ptr TexturePack::GetTexture() const
{
  return m_texture;
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
  return iFind != m_sprites.end() ? &iFind->second : nullptr;
}

//==============================================================================
inline
const Sprite* TexturePack::Get( uint32_t hash ) const
{
  SpriteMap::const_iterator  iFind(m_sprites.find(hash));
  return iFind != m_sprites.end() ? &iFind->second : nullptr;
}

} // xr

#endif // XR_TEXTUREPACK_HPP

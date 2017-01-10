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
  bool              Load(const char* pName,
                      Material::GetCallback pGetCb = Material::Manager::Get,
                      void* pGetCbData = 0);

  Material*         GetMaterial() const;  // no ownership transfer

  int               CountSprites() const;

  Sprite*           Get(const char* pName, bool allowMissing); // no ownership transfer
  Sprite*           Get(const char* pName); // no ownership transfer
  Sprite*           Get(uint32_t hash); // no ownership transfer

  const Sprite*     Get(const char* pName, bool allowMissing) const; // no ownership transfer
  const Sprite*     Get(const char* pName) const;
  const Sprite*     Get(uint32_t hash) const;

  const SpriteMap&  GetSprites() const;

  void              ScaleSprites(float x);
  void              ScaleSprites(float x, float y);

  void              Clear();

protected:
  // data
  Material*  m_pMaterial;  // no ownership
  SpriteMap  m_sprites;
};

//==============================================================================
// implementation
//==============================================================================
inline
Material* TexturePack::GetMaterial() const
{
  return m_pMaterial; // no ownership transfer
}

//==============================================================================
inline
int TexturePack::CountSprites() const
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
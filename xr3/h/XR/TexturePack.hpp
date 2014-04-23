//
// Nuclear Heart Games
// XRhodes
//
// TexturePack.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    06/12/2012
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_TEXTUREPACK_HPP
#define XR_TEXTUREPACK_HPP

#include "Dictionary.hpp"
#include "Sprite.hpp"

namespace XR
{

//==============================================================================
class TexturePack
{
public:
  // types
  typedef Dictionary<uint32, Sprite>  SpriteDictionary;

  // structors
  TexturePack();
  ~TexturePack();

  // general
  bool                    Load(const char* pName,
                            Material::GetCallback pGetCb = Material::Manager::Get,
                            void* pGetCbData = 0);

  Material*               GetMaterial() const;  // no ownership transfer

  int                     CountSprites() const;

  Sprite*                 Get(const char* pName, bool allowMissing); // no ownership transfer
  Sprite*                 Get(const char* pName); // no ownership transfer
  Sprite*                 Get(uint32 hash); // no ownership transfer

  const Sprite*           Get(const char* pName, bool allowMissing) const; // no ownership transfer
  const Sprite*           Get(const char* pName) const;
  const Sprite*           Get(uint32 hash) const;

  const SpriteDictionary& GetSprites() const;

  void                    ScaleSprites(float x);
  void                    ScaleSprites(float x, float y);

  void                    Clear();

protected:
  // data
  Material*         m_pMaterial;  // no ownership
  SpriteDictionary  m_sprites;
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
const TexturePack::SpriteDictionary& TexturePack::GetSprites() const
{
  return m_sprites;
}

//==============================================================================
inline
Sprite* TexturePack::Get( uint32 hash )
{
  return m_sprites.find_value(hash);
}

//==============================================================================
inline
const Sprite* TexturePack::Get( uint32 hash ) const
{
  return m_sprites.find_value(hash);
}

} // XR

#endif // XR_TEXTUREPACK_HPP
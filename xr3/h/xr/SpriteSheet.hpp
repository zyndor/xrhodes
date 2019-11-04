#ifndef XR_SPRITESHEET_HPP
#define XR_SPRITESHEET_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Asset.hpp"
#include "Sprite.hpp"
#include <vector>
#include <algorithm>

namespace xr
{

//==============================================================================
///@brief A set of sprites keyed to (32-bits hashes of their) names plus a path
/// to an image file.
///@par SpriteSheets are generated from TexturePacker's Generic XML format files
/// where the extension is changed to ".sprites".
///@note Creating a texture from the image file is client code responsibility.
class SpriteSheet: public Asset
{
public:
  XR_ASSET_DECL(SpriteSheet)

  //types
  struct Entry
  {
    static bool Compare(Entry const& entry, uint32_t key);

    uint32_t mKey;
    Sprite mSprite;
  };

  using SpriteVector = std::vector<Entry>;

  // general
  ///@return The path of the image that the SpriteSheet is based on.
  FilePath const& GetImagePath() const;

  ///@brief Retrieves the sprite with the key @a hash.
  ///@note If a sprite with the given hash doesn't exist, the result is undefined
  /// (in debug builds, an assert will be tripped).
  Sprite const& Get(uint32_t hash) const;

  ///@brief Retrieves the sprite with the key that the given @a name hashes to.
  ///@note If a sprite with the given name doesn't exist, the result is undefined
  /// (in debug builds, an assert will be tripped).
  Sprite const& Get(char const* name) const;

  ///@brief Retrieves the sprite with the key @a hash.
  ///@note If a sprite with the given hash doesn't exist, the result is undefined
  /// (in debug builds, an assert will be tripped).
  Sprite& Get(uint32_t hash);

  ///@brief Retrieves the sprite with the key that the given @a name hashes to.
  ///@note If a sprite with the given name doesn't exist, the result is undefined
  /// (in debug builds, an assert will be tripped).
  Sprite& Get(char const* name);

  ///@brief Attempts to get the sprite with the key @a hash.
  ///@return Pointer to the sprite if found, or nullptr if not.
  Sprite const* TryGet(uint32_t hash) const;

  ///@brief Attempts to get the sprite with the key that the given @a name hashes to.
  ///@return Pointer to the sprite if found, or nullptr if not.
  Sprite const* TryGet(char const* name) const;

  ///@brief Attempts to get the sprite with the key @a hash.
  ///@return Pointer to the sprite if found, or nullptr if not.
  Sprite* TryGet(uint32_t hash);

  ///@brief Attempts to get the sprite with the key that the given @a name hashes to.
  ///@return Pointer to the sprite if found, or nullptr if not.
  Sprite* TryGet(char const* name);

  ///@return The immutable list of sprites
  SpriteVector const&  GetSprites() const;

  ///@brief Performs operation @a fn on each of the sprites.
  template <typename Fn>
  void ForEachSprite(Fn fn);

protected:
  // data
  FilePath mImagePath;
  SpriteVector mSprites;

  // internal
  bool OnLoaded(Buffer buffer) override;
  void OnUnload() override;
};

//==============================================================================
// inline implementation
//==============================================================================
inline
Sprite const& SpriteSheet::Get(uint32_t hash) const
{
  auto iFind = std::lower_bound(mSprites.begin(), mSprites.end(), hash, Entry::Compare);
  XR_ASSERT(SpriteSheet, iFind != mSprites.end() && iFind->mKey == hash);
  return iFind->mSprite;
}

//==============================================================================
inline
Sprite& SpriteSheet::Get(uint32_t hash)
{
  auto iFind = std::lower_bound(mSprites.begin(), mSprites.end(), hash, Entry::Compare);
  XR_ASSERT(SpriteSheet, iFind != mSprites.end() && iFind->mKey == hash);
  return iFind->mSprite;
}

//==============================================================================
inline
Sprite const* SpriteSheet::TryGet(uint32_t hash) const
{
  auto iFind = std::lower_bound(mSprites.begin(), mSprites.end(), hash, Entry::Compare);
  return iFind != mSprites.end() && iFind->mKey == hash ? &iFind->mSprite : nullptr;
}

//==============================================================================
inline
Sprite* SpriteSheet::TryGet(uint32_t hash)
{
  auto iFind = std::lower_bound(mSprites.begin(), mSprites.end(), hash, Entry::Compare);
  return iFind != mSprites.end() && iFind->mKey == hash ? &iFind->mSprite : nullptr;
}

//==============================================================================
inline
SpriteSheet::SpriteVector const& SpriteSheet::GetSprites() const
{
  return mSprites;
}

//==============================================================================
template<typename Fn>
inline
void SpriteSheet::ForEachSprite(Fn fn)
{
  for (auto& sp: mSprites)
  {
    fn(sp.mSprite);
  }
}

//==============================================================================
inline
bool SpriteSheet::Entry::Compare(Entry const& entry, uint32_t key)
{
  return entry.mKey < key;
}

} // xr

#endif //XR_SPRITESHEET_HPP

#ifndef XR_FONT_HPP
#define XR_FONT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Texture.hpp"
#include "Asset.hpp"
#include "AABB.hpp"
#include "IMaterialisable.hpp"
#include "xr/math/Vector2.hpp"
#include <map>

namespace xr
{

//==============================================================================
///@brief A Font asset, which supports bitmap or SDF based usage. Fonts are built
/// from .fnt files, which are XON documents  with the following properties:
/// font: path to the .ttf, .ttc or .otf file to generate glyphs from;
/// codePoints: hyphen-separated ranges or individual code points to import, as
///   either a literal ('a') or a hex value (0x61). Note that even the pairs are
///   single string (XON) values, therefore they either need to be enclosed in
///   quotes, or there must be no space around the hyphen.
/// cacheSize: the pixel width and height of the texture cache that the Font is
///   going to maintain; optional, default: 1024.
/// fontSize: the pixel height the glyphs should be, including 1/8 SDF padding;
///   optional, default: 128.
/// fontIndex: the index of the font from a .ttc file (.ttf files only contain
///   a single font); optional, default: 0 (first font).
/// sdfSize: the amount of pixels the Signed Distance Field will extend from
///   the glyph; optional, default: 0 - this means a bitmap font. Maximum: (font
///   size / 2) - 1.
///@note The bitmap / SDF use requires an appropriate shader.
///@note Bitmap fonts degrade quality when put through scaling / rotation, while
/// SDF fonts struggle to correctly represent sharp corners; pick your poison.
class Font: public Asset, public IMaterialisable
{
public:
  XR_ASSET_DECL(Font)

  // types
  ///@brief Glyph size information. All dimensions are pixels.
  struct  Glyph
  {
    uint32_t codePoint;
    float width;
    float height;
    float advance;
    float xBearing;
    float yBearing;
    Px fieldWidth;
    Px fieldHeight;
    uint32_t dataOffset;  // -1 means invalid; it should be accompanied by field width / height of 0.

    bool operator<(Glyph const& rhs) const
    {
      return codePoint < rhs.codePoint;
    }
  };

  ///@brief A glyph that's in the texture cache.
  struct CachedGlyph
  {
    Glyph const* glyph;
    AABB uvs;
  };

  // general
  ///@return The height of a line in pixels / world units, i.e. the amount of
  /// vertical space you need for a single line of text.
  float GetLineHeight() const;

  ///@return The distance from the baseline to the top of the tallest glyph.
  float GetAscent() const;

  ///@return The amount you need to scale the metrics of this Font and its
  /// Glyphs for the given line height.
  float GetScaleForHeight(float heightPixels) const;

  ///@return Glyph data, if it exists for the given @a codePoint in the Font.
  Glyph const* GetGlyph(uint32_t codePoint) const;

  ///@return The binary blob of glyph bitmaps, indexible into using Glyph::dataOffset,
  /// fieldWidth and fieldHeight.
  uint8_t const* GetGlyphBitmapData() const;

  ///@brief Attempts to copy a glyph's data to the cache and return its UVs.
  ///@return A pointer to the UVs of of the glyph over the cache texture; nullptr
  /// in case of a failure, which might mean a failure to allocate space, or that
  /// the font didn't have the glyph.
  ///@note There is no guarantee for the returned pointer to remain valid past
  /// a subsequent call to CacheGlyph(), therefore you should not store it.
  CachedGlyph const* CacheGlyph(uint32_t codePoint);

  ///@brief Updates the texture used for caching the glyphs; this needs to be
  /// called after caching glyphs.
  ///@note It will only re-upload the texture if any glyphs have been successfully
  /// cached since last call.
  void UpdateCache();

  ///@brief Removes all cached glyphs.
  void ClearCache();

  ///@brief Returns the Texture used by the Font as glyph cache.
  Texture::Ptr GetTexture() const override;

protected:
  // types
  typedef std::map<uint32_t, Glyph>  GlyphMap;

  struct CachedGlyphInternal
  {
    uint8_t* buffer;
    CachedGlyph glyph;
  };

  // data
  float m_lineHeight;
  float m_ascent;

  std::vector<CachedGlyphInternal> m_cachedGlyphs;
  Texture::Ptr m_texture;
  class TextureCache* m_textureCache = nullptr;
  bool m_cacheDirty = false;

  GlyphMap  m_glyphs;
  std::vector<uint8_t>  m_glyphBitmaps;

  Px m_cacheSideSizePixels;

  // internal
  virtual bool OnLoaded(Buffer buffer) override;
  virtual void OnUnload() override;

  CachedGlyphInternal* CacheGlyphInternal(uint32_t codePoint);
};

//==============================================================================
// implementation
//==============================================================================
inline
float Font::GetLineHeight() const
{
  return m_lineHeight;
}

//==============================================================================
inline
float Font::GetAscent() const
{
  return m_ascent;
}

//==============================================================================
inline
float Font::GetScaleForHeight(float hPixels) const
{
  return hPixels / m_lineHeight;
}

//==============================================================================
inline
Font::Glyph const*  Font::GetGlyph(uint32_t codePoint) const
{
  GlyphMap::const_iterator iFind(m_glyphs.find(codePoint));
  return iFind != m_glyphs.end() ? &iFind->second : nullptr;
}

//==============================================================================
inline
uint8_t const* Font::GetGlyphBitmapData() const
{
  return m_glyphBitmaps.data();
}

} // xr

#endif  //XR_FONT_HPP

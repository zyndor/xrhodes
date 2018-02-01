#ifndef XR_FONT_HPP
#define XR_FONT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "BasicMesh.hpp"
#include "Material.hpp"
#include "Asset.hpp"
#include "AABB.hpp"
#include "Vector2.hpp"
#include <map>

namespace XR
{

//==============================================================================
///@brief A Signed Distance Field based Font asset. A Font is built from an .fnt
/// file, which is a XON document with the following properties:
/// font: path to the .ttf, .ttc or .otf file to generate glyphs from;
/// codePoints: hyphen-separated ranges or individual code points to import, as
///   either a literal ('a') or a hex value (0x61).
/// cacheSize: the pixel width and height of the texture cache that the Font
///   is going to maintain; optional, default: 1024.
/// fontSize: the pixel height the glyphs should be, including 1/8 SDF padding;
///   optional, default: 128.
/// fontIndex: the index of the font from a .ttc file (.ttf files only contain
///   a single font); optional, default: 0 (first font).
/// sdfSize: the amount of pixels the Signed Distance Field will extend from
///   the glyph; optional, default: 4. Minimum: 1. Maximum: (font size / 2) - 1.
/// shader: path to the shader to use for the font.
class Font: public Asset
{
public:
  XR_ASSET_DECL(Font)

  // types
  ///@brief Glyph size information.
  struct  Glyph
  {
    uint32_t codePoint;
    float width;
    float height;
    float advance;
    float xBearing;
    float yBearing;
    uint32_t fieldWidth;
    uint32_t fieldHeight;
    uint32_t dataOffset;

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

  // static
  ///@brief Descriptor of a default Shader, for Fonts that don't define one.
  /// If not set [by application code], then it will be set to the first Shader
  /// that's defined by a Font that's being loaded.
  ///@note Must be of a Shader that's loaded and managed by the Asset::Manager.
  static Asset::Descriptor<Shader> s_descDefaultShader;

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
  const Glyph* GetGlyph(uint32_t codePoint) const;

  ///@brief Attempts to copy a glyph's data to the cache and return its UVs.
  ///@return A pointer to the UVs of of the glyph over the cache texture; nullptr
  /// in case of a failure, which might mean a failure to allocate space, or that
  /// the font didn't have the glyph.
  ///@note There is no guarantee for the returned pointer to remain valid past
  /// a subsequent call to CacheGlyph(), therefore you should not store it.
  CachedGlyph const* CacheGlyph(uint32_t codePoint);

  ///@brief Removes all cached glyphs.
  void ClearCache();

  ///@brief Returns the material managed by the Font. If one or more glyphs
  /// has been cached since the last call to it, it will update the underlying
  /// texture.
  Material::Ptr GetMaterial();

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
  Material::Ptr m_material;
  class TextureCache* m_textureCache = nullptr;
  bool m_cacheDirty = false;

  GlyphMap  m_glyphs;
  std::vector<uint8_t>  m_glyphBitmaps;

  int m_cacheSideSizePixels;

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

} // XR

#endif  //XR_FONT_HPP

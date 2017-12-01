#ifndef XR_FONT_HPP
#define XR_FONT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "AABB.hpp"
#include "Material.hpp"
#include <map>

// TODO: remove
#include "ResManager.hpp"

namespace XR
{

//==============================================================================
class Font
{
  XR_MANAGED_DECL(Font)

public:
  // types
  enum 
  {
    kNumBasicGlyphs = 256
  };

  struct  Glyph
  {
    float w;
    float h;
    float xOffs;
    float yOffs;
    float xAdvance;
    
    AABB  uvs;

    void  Scale(float s);
  };

  // structors
  Font();
  ~Font();

  // general
  ///@deprecated Asset::Manager integration in progress.
  ///@brief Attempts to load a Font synchronously. Requires a material in the
  /// same location and name (but with '.mtl' extension) as the texture in its
  /// definition.
  bool            Load(char const* name, Asset::FlagType flags = 0);  // full path; font name will be the filename without path and extension.

  float           GetSize() const;
  float           GetBase() const;
  float           CalcHeight(int numLines) const;
  const Glyph*    GetGlyph(int chr) const;

  Material::Ptr   GetMaterial() const;  // no ownership transfer

  void            Scale(float s); 
  void            Clear();

protected:
  // types
  typedef std::map<int, Glyph>  GlyphMap;

  // data
  float           m_size;
  float           m_base; // extra space required below last line
  
  Glyph           m_arGlyphs[kNumBasicGlyphs];
  GlyphMap        m_dGlyphs;

  Material::Ptr   m_material;  // no ownership
};

//==============================================================================
// implementation
//==============================================================================
inline
float Font::GetSize() const
{
  return m_size;
}

//==============================================================================
inline
float Font::GetBase() const
{
  return m_base;
}

//==============================================================================
inline
const Font::Glyph*  Font::GetGlyph(int chr) const
{
  XR_ASSERT(Font, chr >= 0);
  if (chr < kNumBasicGlyphs)
  {
    return m_arGlyphs + chr;
  }
  else
  {
    GlyphMap::const_iterator iFind(m_dGlyphs.find(chr));
    return iFind != m_dGlyphs.end() ? &iFind->second : 0;
  }
}

//==============================================================================
inline
float Font::CalcHeight(int numLines) const
{
  XR_ASSERT(Font::Adapter, numLines >= 0);
  return ceilf(numLines * m_size + m_base);
}

//==============================================================================
inline
Material::Ptr Font::GetMaterial() const
{
  return m_material;
}

} // XR

#endif  //XR_FONT_HPP
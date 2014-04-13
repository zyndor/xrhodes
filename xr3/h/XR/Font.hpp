//
// Nuclear Heart Games
// XRhodes
//
// Font.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    21/11/2012
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_FONT_HPP
#define XR_FONT_HPP

#include "Sprite.hpp"
#include "Dictionary.hpp"

namespace XR
{

//==============================================================================
class Font
{
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
  };

  // structors
  Font();
  ~Font();

  // general
  bool            Load(const char* pName, Material::GetCallback pGetMaterialCb,
                    void* pGetMaterialCbData);  // full path; get filename without extension as font name

  float           GetSize() const;
  float           GetBase() const;
  float           CalcHeight(int numLines) const;
  const Glyph*    GetGlyph(int chr) const;

  Material*       GetMaterial() const;  // no ownership transfer

  void            Clear();

protected:
  // types
  typedef Dictionary<int, Glyph>  GlyphDictionary;
  typedef Dictionary<int, AABB>   AABBDictionary;

  // data
  float           m_size;
  float           m_base; // extra space required below last line
  
  Glyph           m_arGlyphs[kNumBasicGlyphs];
  GlyphDictionary m_dGlyphs;

  Material*       m_pMaterial;  // no ownership
};

//==============================================================================
/*
  implementation
*/
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
  if(chr < kNumBasicGlyphs)
  {
    return m_arGlyphs + chr;
  }
  else
  {
    return m_dGlyphs.find_value(chr);
  }
}

//==============================================================================
inline
float Font::CalcHeight(int numLines) const
{
  XR_ASSERT(Font::Adapter, numLines >= 0);
  return numLines * m_size + m_base;
}

//==============================================================================
inline
Material* Font::GetMaterial() const
{
  return m_pMaterial;
}

} // XR

#endif  //XR_FONT_HPP
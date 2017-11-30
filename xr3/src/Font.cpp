//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <limits>
#include <iostream>
#include "XR/FileBuffer.hpp"
#include "XR/Font.hpp"
#include "XR/utils.hpp"
#include "XR/debug.hpp"

namespace XR
{

//==============================================================================
enum
{
  TAG_SIZE,
  TAG_OUTLINE,
  TAG_BASE,
  TAG_SCALE_W,
  TAG_SCALE_H,
  TAG_FILE,
  TAG_CHARS_COUNT,
  TAG_CHAR_ID,
  TAG_X,
  TAG_Y,
  TAG_WIDTH,
  TAG_HEIGHT,
  TAG_XOFFSET,
  TAG_YOFFSET,
  TAG_XADVANCE,
  kNumAttribs  
};

//==============================================================================
static const char* const karTag[]=
{
  "size=",
  "outline=",
  "base=",
  "scaleW=",
  "scaleH=",
  "file=",
  "chars count=",
  "char id=",
  "x=",
  "y=",
  "width=",
  "height=",
  "xoffset=",
  "yoffset=",
  "xadvance="
};

//==============================================================================
static const char* FindValue(const char* pBuffer, const char* pKey,
    const char*& pResult)
{
  XR_ASSERT(FindValue, pBuffer != nullptr);
  XR_ASSERT(FindValue, pKey != nullptr);
  const char* pNext = strstr(pBuffer, pKey);
  if (pNext != nullptr)
  {
    size_t keySize(strlen(pKey));
    pResult = pNext + keySize;

    pNext = strpbrk(pResult, " \n\t\0");
  }
  else
  {
    pResult = 0;  
  }
  return pNext;
}

//==============================================================================
void  Font::Glyph::Scale(float s)
{
  w *= s;
  h *= s;
  xOffs *= s;
  yOffs *= s;
  xAdvance *= s;
}

//==============================================================================
Font::Font()
: m_size(.0f),
  m_base(.0f),
  //m_arGlyphs(),
  m_dGlyphs()
{}

//==============================================================================
Font::~Font()
{}

//==============================================================================
bool  Font::Load(char const* name, Asset::FlagType flags)
{
  XR_ASSERT(Font, name != nullptr);
  XR_ASSERT(Font, strlen(name) > 0);

  Clear();

  XR_ASSERT(Font, name != nullptr);
  FileBuffer  file;
  bool  success(file.Open(name, false));
  if (!success)
  {
    return false;
  }

  file.Close();
  if (success)
  {
    const char* pRead(file.CastData<char>());
    const char* pTempRead;
    const char* pValue;

    pTempRead = FindValue(pRead, karTag[TAG_SIZE], pValue);
    success = success && (pValue != nullptr);
    if (success)
    {
      m_size = float(atof(pValue));
      pRead = pTempRead;
    }

    float fOutline(.0f);
    pTempRead = FindValue(pRead, karTag[TAG_OUTLINE], pValue);
    success = success && (pValue != nullptr);
    if (success)
    {
      fOutline = float(atof(pValue));
      m_size += fOutline * 2.0f;
      pRead = pTempRead;
    }
    
    pTempRead = FindValue(pRead, karTag[TAG_BASE], pValue);
    success = success && (pValue != nullptr);
    if (success)
    {
      m_base = m_size - float(atof(pValue));
      pRead = pTempRead;
    }

    float fTexWidth(.0f);
    pTempRead = FindValue(pRead, karTag[TAG_SCALE_W], pValue);
    success = success && (pValue != nullptr);
    if (success)
    {
      fTexWidth = float(atof(pValue));
      if (fTexWidth > .0f)
      {
        fTexWidth = 1.0f / fTexWidth;
      }
      pRead = pTempRead;
    }

    float fTexHeight(.0f);
    pTempRead = FindValue(pRead, karTag[TAG_SCALE_H], pValue);
    success = success && (pValue != nullptr);
    if (success)
    {
      fTexHeight = float(atof(pValue));
      if (fTexHeight > .0f)
      {
        fTexHeight = 1.0f / fTexHeight;
      }
      pRead = pTempRead;
    }

    pTempRead = FindValue(pRead, karTag[TAG_FILE], pValue);
    success = success && (pValue != nullptr);
    FilePath texturePath;
    if (success)
    {
      if (pValue[0] == '"')
      {
        ++pValue;
      }

      const char* pClosingQuote(strchr(pValue, '\"'));
      const char* pPeriod(strchr(pValue, '.'));

      success = !(pClosingQuote == nullptr || pPeriod == nullptr) &&
        pPeriod < pClosingQuote &&
        (pClosingQuote - pValue) < texturePath.capacity();
      if (success)
      {
        texturePath = name;
        texturePath.Up();
        texturePath.AppendDirSeparator();
        texturePath.append(pValue, pPeriod - pValue);
        texturePath += ".mtl";
      }
      else
      {
        XR_TRACE(Font, ("Failed to parse '%s' for %s", karTag[TAG_FILE], name));
      }
    }
    else
    {
      XR_TRACE(Font, ("%s must define '%s'", name, karTag[TAG_FILE]));
    }

    if (success)
    {
      m_material = Asset::Manager::Load<Material>(texturePath, flags |
        Asset::LoadSyncFlag | Asset::UnmanagedFlag);
      success = !CheckAnyMaskBits(m_material->GetFlags(), Asset::ErrorFlag);
      if (!success)
      {
        XR_TRACE(Font, ("Failed to load '%s' for font '%s'", texturePath.c_str(),
          name));
      }
    }

    if (success)
    {
      pTempRead = FindValue(pRead, karTag[TAG_CHARS_COUNT], pValue);
    }
    
    success = success && pValue != nullptr;
    if (!success)
    {
      XR_TRACE(Font, ("%s is a required attribute.", karTag[TAG_CHARS_COUNT]));
    }

    int numChars(0);
    success = success && sscanf(pValue, "%d", &numChars) > 0;
    if (success)
    {
      pRead = pTempRead;
    }

    int     chr;
    Glyph*  pGlyph;
    while (success && numChars > 0)
    {
      pTempRead = FindValue(pRead, karTag[TAG_CHAR_ID], pValue);

      success = pValue != nullptr;
      if (success)
      {
        chr = atoi(pValue);
        if (chr >= 0 && chr < kNumBasicGlyphs)
        {
          pGlyph = m_arGlyphs + chr;
        }
        else
        {
          Glyph&  g(m_dGlyphs[chr]);
          pGlyph = &g;
        }
        
        float x;
        pTempRead = FindValue(pTempRead, karTag[TAG_X], pValue);
        success = pValue != nullptr;
        if (!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karTag[TAG_X]));
          break;
        }
        x = float(atof(pValue));

        float y;
        pTempRead = FindValue(pTempRead, karTag[TAG_Y], pValue);
        success = pValue != nullptr;
        if (!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karTag[TAG_Y]));
          break;
        }
        y = float(atof(pValue));

        pTempRead = FindValue(pTempRead, karTag[TAG_WIDTH], pValue);
        success = pValue != nullptr;
        if (!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karTag[TAG_WIDTH]));
          break;
        }
        pGlyph->w = float(atof(pValue));

        pTempRead = FindValue(pTempRead, karTag[TAG_HEIGHT], pValue);
        success = pValue != nullptr;
        if (!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karTag[TAG_HEIGHT]));
          break;
        }
        pGlyph->h = float(atof(pValue));

        pTempRead = FindValue(pTempRead, karTag[TAG_XOFFSET], pValue);
        success = pValue != nullptr;
        if (!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karTag[TAG_XOFFSET]));
          break;
        }
        pGlyph->xOffs = float(atof(pValue)) + fOutline;

        pTempRead = FindValue(pTempRead, karTag[TAG_YOFFSET], pValue);
        success = pValue != nullptr;
        if (!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karTag[TAG_YOFFSET]));
          break;
        }
        pGlyph->yOffs = float(atof(pValue)) + fOutline;

        pTempRead = FindValue(pTempRead, karTag[TAG_XADVANCE], pValue);
        success = pValue != nullptr;
        if (!success)
        {
          XR_TRACE(Font, ("%s is a required attribute.", karTag[TAG_XADVANCE]));
          break;
        }
        pGlyph->xAdvance = float(atof(pValue)) + fOutline * 2.0f;

        // uvs
        pGlyph->uvs.left = x * fTexWidth;
        pGlyph->uvs.right = (x + pGlyph->w) * fTexWidth;
        pGlyph->uvs.top = y * fTexHeight;
        pGlyph->uvs.bottom = (y + pGlyph->h) * fTexHeight;

        pRead = pTempRead;
      }
      --numChars;
    }
  }

  return success;
}

//==============================================================================
void  Font::Scale(float s)
{
  m_size *= s;
  m_base *= s;

  for(Glyph* i0(m_arGlyphs), * i1(m_arGlyphs + kNumBasicGlyphs); i0 != i1; ++i0)
  {
    i0->Scale(s);
  }
  
  for(GlyphMap::iterator i0(m_dGlyphs.begin()), i1(m_dGlyphs.end()); i0 != i1; ++i0)
  {
    i0->second.Scale(s);
  }
}

//==============================================================================
void  Font::Clear()
{
  m_size = .0f;
  m_base = .0f;
  
  memset(m_arGlyphs, 0x00, sizeof(m_arGlyphs));
  m_dGlyphs.clear();
}
  
} // XR

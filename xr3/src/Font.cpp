//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifdef ENABLE_ASSET_BUILDING
#include "RangeTree.hpp"
#include "SdfBuilder.hpp"
#endif

#include "xr/TextureCache.hpp"
#include "xr/XonBuildTree.hpp"
#include "xr/FileBuffer.hpp"
#include "xr/Font.hpp"
#include "xr/BufferReader.hpp"
#include "xr/streamutils.hpp"
#include "xr/stringutils.hpp"
#include "xr/utils.hpp"
#include "xr/debug.hpp"
#include "stb_truetype.h"
#include <limits>
#include <iostream>
#include <list>
#include <regex>

//#define ENABLE_GLYPH_DEBUG

#ifdef ENABLE_GLYPH_DEBUG
#include "xr/Image.hpp"
#endif

#define LTRACE(format) XR_TRACE(Font, format)
#define LTRACEIF(condition, format) XR_TRACEIF(Font, condition, format)

namespace xr
{

//==============================================================================
XR_ASSET_DEF(Font, "xfnt", 2, "fnt")

/*
  {
    font: "ComicSans.ttf",
    fontSize: 128,
    fontIndex: 0,
    codePoints: {
      '\\'',          # ascii character, escaped
      '!'-'z',        # ascii character range
      'Z'-'A',        # that's ok, we understand
      0x0020,         # code point
      0x0065-0x0091,  # code point range
      0x0091-0x0065,  # guess what, still works
      0x0100-'a'      # come at me with everything you've got.
    }
  }
*/

#ifdef ENABLE_ASSET_BUILDING
namespace {

const int kMinFontSize = 32;
const int kDefaultFontSize = 128;

const int kMinSdfSize = 1;
const int kDefaultSdfSize = 4;

const int kMinCacheSize = 256;
const int kDefaultCacheSize = 1024;

const float kUnitsToPixel = 1.0f / 64.0f;

const std::string kStrValueRegex("('(\\\\?.)'|(0x[0-9a-z]{1,4}))");
const std::regex kValueRegex(kStrValueRegex + "( *- *" + kStrValueRegex + ")?", std::regex::icase);

bool ParseLiteralChar(std::string const& value, uint32_t& c)
{
  // Escaped characters, other than ' and \, are not supported via the literal path.
  // Since these are also non-printing, this shouldn't be too much of a restriction.
  bool success = value.size() == 1 || (value[0] == '\\' &&
    (value[1] == '\\' || value[1] == '\''));  // == 1 : we know it's not empty.
  if(success)
  {
    c = value[value.size() - 1];
  }
  else
  {
    LTRACE(("Invalid code point literal: '%s'", value.c_str()));
  }
  return success;
}

bool ParseHexCodePoint(std::string const& value, uint32_t& c)
{
  bool success = !!(std::istringstream(value) >> std::ws >> std::hex >> c);
  LTRACEIF(!success, ("Failed to parse hex code point value: %s",
    value.c_str()));
  return success;
}

bool ParseRange(char const* def, std::unique_ptr<RangeTree>& range)
{
  std::cmatch results;
  bool success = std::regex_match(def, results, kValueRegex);
  if (success)
  {
    RangeTree r;
    std::string match = results[2].str();
    if (!match.empty())
    {
      success = ParseLiteralChar(match, r.start);
    }
    else
    {
      success = ParseHexCodePoint(results[3].str(), r.start); // results[3].str() cannot be empty, since it's matched the regex.
    }

    if(success)
    {
      match = results[6].str();
      if (!match.empty())
      {
        success = ParseLiteralChar(match, r.end);
      }
      else
      {
        match = results[7].str();
        if(!match.empty())
        {
          success = ParseHexCodePoint(match, r.end);
        }
        else
        {
          r.end = r.start;
        }
      }
    }

    if (success)
    {
      if (r.end < r.start)
      {
        std::swap(r.start, r.end);
      }

      ++r.end;
      success = r.end > r.start;
      if(success)
      {
        if(range)
        {
          range->Add(r);
        }
        else
        {
          range.reset(new RangeTree(r));
        }
      }
    }
  }
  return success;
}

XR_ASSET_BUILDER_DECL(Font)

XR_ASSET_BUILDER_BUILD_SIG(Font)
{
  XonParser::State state;
  auto root = XonBuildTree(buffer.As<char const>(), buffer.size, &state);
  bool success = root != nullptr;
  LTRACEIF(!success,
    ("%s: failed to parse XON somewhere around row %d, column %d.",
      rawNameExt, state.row, state.column));

  // Get font file path, read file.
  FileBuffer ttfData;
  if(success)
  {
    try
    {
      auto& fontPath = root->Get("font").ToValue();
      success = ttfData.Open(fontPath.GetString(), false);
      LTRACEIF(!success, ("%s: failed to load '%s'.", rawNameExt, fontPath.GetString()));
    }
    catch(XonEntity::Exception&)
    {
      success = false;
      LTRACE(("%s: missing definition for '%s'", rawNameExt, "font"));
    }
  }

  // Get code point ranges.
  std::unique_ptr<RangeTree> codePointsRoot;
  if(success)
  {
    try
    {
      auto& codePoints = root->Get("codePoints").ToObject();
      for(size_t i0 = 0, i1 = codePoints.GetNumElements(); i0 < i1; ++i0)
      {
        try
        {
          char const* rangeDef = codePoints[i0].ToValue().GetString();
          if(!ParseRange(rangeDef, codePointsRoot))
          {
            LTRACE(("%s: '%s' is not a valid code point range.", rawNameExt,
              rangeDef));
          }
        }
        catch(XonEntity::Exception const&)
        {
          LTRACE(("%s: ignoring code point entry %d: should be a value.", rawNameExt, i0));
        }
      }
    }
    catch(XonEntity::Exception const&)
    {
      success = false;
      LTRACE(("%s: missing definition for '%s'", rawNameExt, "codePoints"));
    }
  }

  // Handle some optional attributes.
  int numFonts = 0;
  if (success)
  {
    numFonts = stbtt_GetNumberOfFonts(ttfData.GetData());
    success = numFonts > 0;
    LTRACEIF(!success, ("%s: failed to get number of fonts (%d).", rawNameExt,
      numFonts));
  }

  int fontIndex = 0;
  int fontSize = kDefaultFontSize; // includes padding for SDF, see below.
  int sdfSize = kDefaultSdfSize;
  int cacheSize = kDefaultCacheSize;
  Asset::HashType shaderHash = 0;
  XonEntity* xon = nullptr;
  if (success && (xon = root->TryGet("fontIndex")))
  {
    try
    {
      auto& indexValue = xon->ToValue();
      if (!StringTo(indexValue.GetString(), fontIndex) || fontIndex < 0 ||
        fontIndex >= numFonts)
      {
        fontIndex = 0;
        LTRACE(("%s: '%s' is invalid for %s; defaulting to %d.", rawNameExt,
          indexValue.GetString(), "fontIndex", fontIndex));
      }
    }
    catch (XonEntity::Exception const& e)
    {
      XR_ASSERT(Font, e.type == XonEntity::Exception::Type::InvalidType);
      success = false;
      LTRACE(("%s: %s has invalid type.", rawNameExt, "fontIndex"));
    }
  }

  if (success && (xon = root->TryGet("fontSize")))
  {
    try
    {
      auto& sizeValue = xon->ToValue();
      if (!StringTo(sizeValue.GetString(), fontSize))
      {
        LTRACE(("%s: failed to parse %s for %s, defaulting to %d.", rawNameExt,
          sizeValue.GetString(), "fontSize", fontSize));
      }
      else if (fontSize < kMinFontSize)
      {
        LTRACE(("%s: clamped %s (%d) to minimum (%d).", "fontSize", rawNameExt,
          fontSize, kMinFontSize));
        fontSize = kMinFontSize;
      }
    }
    catch (XonEntity::Exception const& e)
    {
      XR_ASSERT(Font, e.type == XonEntity::Exception::Type::InvalidType);
      success = false;
      LTRACE(("%s: %s has invalid type.", rawNameExt, "fontSize"));
    }
  }

  if (success && (xon = root->TryGet("sdfSize")))
  {
    try
    {
      auto& sizeValue = xon->ToValue();
      if (!StringTo(sizeValue.GetString(), sdfSize))
      {
        LTRACE(("%s: failed to parse %s for %s, defaulting to %d.", rawNameExt,
          sizeValue.GetString(), "sdfSize", sdfSize));
      }
      else if (sdfSize < kMinSdfSize)
      {
        LTRACE(("%s: clamped %s (%d) to minimum (%d).", "sdfSize", rawNameExt,
          sdfSize, kMinSdfSize));
        sdfSize = kMinSdfSize;
      }
      else
      {
        const int maxSdfSize = (fontSize / 2) - 1;
        if (sdfSize > maxSdfSize)
        {
          LTRACE(("%s: clamped %s (%d) to maximum (%d).", "sdfSize", rawNameExt,
            sdfSize, maxSdfSize));
          sdfSize = maxSdfSize;
        }
      }
    }
    catch (XonEntity::Exception const& e)
    {
      if (e.type == XonEntity::Exception::Type::InvalidType)
      {
        XR_ASSERT(Font, e.type == XonEntity::Exception::Type::InvalidType);
        success = false;
        LTRACE(("%s: %s has invalid type.", rawNameExt, "sdfSize"));
      }
    }
  }

  if (success && (xon = root->TryGet("cacheSize")))
  {
    try
    {
      auto& sizeValue = xon->ToValue();
      if (!StringTo(sizeValue.GetString(), cacheSize))
      {
        LTRACE(("%s: failed to parse %s for %s, defaulting to %d.", rawNameExt,
          sizeValue.GetString(), "cacheSize", cacheSize));
      }
      else if (cacheSize < kMinCacheSize)
      {
        LTRACE(("%s: clamped %s (%d) to minimum (%d).", "cacheSize", rawNameExt,
          cacheSize, kMinCacheSize));
        cacheSize = std::min(cacheSize, kMinCacheSize);
      }
    }
    catch (XonEntity::Exception const& e)
    {
      XR_ASSERT(Font, e.type == XonEntity::Exception::Type::InvalidType);
      success = false;
      LTRACE(("%s: %s has invalid type.", rawNameExt, "cacheSize"));
    }
  }

  if (success && (xon = root->TryGet("shader")))
  {
    try
    {
      auto& shader = xon->ToValue();
      if (auto shaderPath = shader.GetString())
      {
        dependencies.push_back(shaderPath);
        shaderHash = Asset::Manager::HashPath(dependencies.back());
      }
    }
    catch (XonEntity::Exception const& e)
    {
      XR_ASSERT(Font, e.type == XonEntity::Exception::Type::InvalidType);
      success = false;
      LTRACE(("%s: %s has invalid type.", rawNameExt, "shader"));
    }
  }

  // Init font data.
  stbtt_fontinfo stbFont;
  if (success)
  {
    int offset = stbtt_GetFontOffsetForIndex(ttfData.GetData(), fontIndex);
    success = stbtt_InitFont(&stbFont, ttfData.GetData(), offset) != 0;
    LTRACEIF(!success, ("%s: failed to initialise font.", rawNameExt));
  }

  // Calculate and write some font metrics. These are in 1/64 sub-pixel units.
  // We're writing pixels.
  int ascent, descent, lineGap;
  float sdfSizeUnits;
  int x0, x1, y0, y1;
  float pixelScale; // reciprocal of total line height, i.e. ascent - descent + lineGap.
  if (success)
  {
    stbtt_GetFontVMetrics(&stbFont, &ascent, &descent, &lineGap);
    const int lineHeightUnits = ascent - descent + lineGap;

    stbtt_GetFontBoundingBox(&stbFont, &x0, &y0, &x1, &y1);
    int heightUnits = y1 - y0;
    sdfSizeUnits = (sdfSize / float(fontSize)) * heightUnits;

    pixelScale = stbtt_ScaleForPixelHeight(&stbFont, float(fontSize - sdfSize * 2));

    success = WriteBinaryStream(lineHeightUnits * kUnitsToPixel, data) &&
      WriteBinaryStream(ascent * kUnitsToPixel, data) &&
      WriteBinaryStream(cacheSize, data);
    LTRACEIF(!success, ("%s: failed to write font metrics.", rawNameExt));
  }

  // Calculate and write number of glyphs
  uint32_t numGlyphs = 0;
  if(success)
  {
    auto iRanges = codePointsRoot->Lowest();
    while (iRanges)
    {
      numGlyphs += iRanges->end - iRanges->start;
      iRanges = iRanges->Higher();
    }

    // generate glyphs
    if (!WriteBinaryStream(numGlyphs, data))
    {
      success = false;
      LTRACE(("%s: failed to write number of glyphs.", rawNameExt));
    }
  }

  // Now calculate SDF from TTF. The reason we're not using stbtt's SDF
  // functionality is that we want to control (and minimize) the allocations.
  if(success)
  {
    const int ascentPixels = int(std::ceil(ascent * pixelScale));

    const int glyphPadding = 1; // We pad the glyph bitmap to simplify comparing neighbour pixels, where we'd need to special case at the edges - reading off by one.
    const int glyphSizePadded = fontSize + glyphPadding * 2;

    std::vector<uint8_t> glyphBitmap(glyphSizePadded * glyphSizePadded);
    uint8_t* glyphBitmapPadded = glyphBitmap.data() + glyphSizePadded + glyphPadding;

    SdfBuilder sdf(fontSize, sdfSize);
    Font::Glyph glyph;
    std::ostringstream glyphBitmaps;
    size_t glyphBytesWritten = 0;
    auto iRanges = codePointsRoot->Lowest();
    while(iRanges)
    {
      for(auto i0 = iRanges->start, i1 = iRanges->end; i0 != i1; ++i0)
      {
        auto const iGlyph = stbtt_FindGlyphIndex(&stbFont, i0);
        if(iGlyph < 0)
        {
          LTRACE(("%s: no glyph found for code point 0x%x.", rawNameExt, i0));
          continue;
        }

        glyph.codePoint = i0;

        // Get glyph metrics.
        int advance;
        int xBearing;
        stbtt_GetGlyphHMetrics(&stbFont, iGlyph, &advance, &xBearing);
        glyph.advance = advance * kUnitsToPixel;
        glyph.xBearing = (xBearing - sdfSizeUnits) * kUnitsToPixel;

        // Get glyph box in font units.
        // Y increases upwards and the origin is the baseline / ascent.
        // Y0 is bottom, Y1 is top.
        stbtt_GetGlyphBox(&stbFont, iGlyph, &x0, &y0, &x1, &y1);

        glyph.width = (x1 - x0 + 2.0f * sdfSizeUnits) * kUnitsToPixel;
        glyph.height = (y1 - y0 + 2.0f * sdfSizeUnits) * kUnitsToPixel;
        glyph.yBearing = (y0 - sdfSizeUnits) * kUnitsToPixel;

        if (isspace(i0)) // if whitespace, don't waste memory.
        {
          glyph.fieldHeight = 0;
          glyph.fieldWidth = 0;
          glyph.dataOffset = -1;
        }
        else
        {
          // Copy glyph bitmap data to an area padded for SDF.
          std::fill(glyphBitmap.begin(), glyphBitmap.end(), 0x00);

          // Get glyph bitmap box coordinates.
          // Y increases downwards and the origin is the baseline / ascent.
          // Y0 is top (ascent + y0 for absolute coords), Y1 is bottom.
          stbtt_GetGlyphBitmapBox(&stbFont, iGlyph, pixelScale, pixelScale,
            &x0, &y0, &x1, &y1);
          int w = x1 - x0;
          int h = y1 - y0;

          auto xBearingPixels = int(std::ceil(glyph.xBearing * pixelScale));
          auto xPixelOffs = sdfSize + xBearingPixels;
          auto yPixelOffs = sdfSize + ascentPixels + y0;
          auto bufferOffset = xPixelOffs + yPixelOffs * glyphSizePadded;

          // Blit the glyph at its absolute position (sdf padding and ascent applies).
          stbtt_MakeGlyphBitmap(&stbFont, glyphBitmapPadded + bufferOffset,
            w, h, glyphSizePadded, pixelScale, pixelScale, iGlyph);

          // Calculate SDF metrics & generate SDF around glyph.
          auto sx0 = std::max(xPixelOffs - sdfSize, 0);
          auto sy0 = std::max(yPixelOffs - sdfSize, 0);  // ascentPixels + y0; never really < 0.
          auto sx1 = std::min(xPixelOffs + w + sdfSize, fontSize);
          auto sy1 = std::min(yPixelOffs + h + sdfSize, fontSize);
          auto sw = sx1 - sx0;
          auto sh = sy1 - sy0;
          bufferOffset = sx0 + sy0 * glyphSizePadded;
          sdf.Generate(glyphBitmapPadded + bufferOffset, glyphSizePadded, sw, sh);
          sdf.ConvertToBitmap(sw, sh, glyphBitmap.data());

          glyph.fieldWidth = sw;
          glyph.fieldHeight = sh;
          XR_ASSERT(Font, glyphBytesWritten < std::numeric_limits<decltype(glyph.dataOffset)>::max());
          glyph.dataOffset = static_cast<uint32_t>(glyphBytesWritten);

          // write glyph bitmap data to other stream.
          auto p = glyphBitmap.data();
#ifdef ENABLE_GLYPH_DEBUG
          Image img;
          img.SetPixelData(p, w, h, 1);

          FilePath path;
          sprintf(path.data(), "glyph-%04x.tga", i0);
          path.UpdateSize();
          img.Save(path, true);
#endif

          if (!WriteRangeBinaryStream<uint32_t>(p, p + (sw * sh), glyphBitmaps,
            &glyphBytesWritten))
          {
            success = false;
            LTRACE(("%s: failed to write glyph bitmap data for 0x%x in '%s'.",
              rawNameExt, i0));
            break;
          }
        }

        // write glyph.
        if(!WriteBinaryStream(glyph, data))
        {
          success = false;
          LTRACE(("%s: failed to write glyph data for 0x%x.", rawNameExt, i0));
          break;
        }
      }

      iRanges = iRanges->Higher();
    }

    if(success && !WriteBinaryStream(static_cast<uint32_t>(glyphBytesWritten), data))
    {
      success = false;
      LTRACE(("%s: failed to write glyp bitmap blob size.", rawNameExt));
    }

    if(success)
    {
      auto blob = glyphBitmaps.str();
      auto p = reinterpret_cast<uint8_t const*>(blob.data());
      success = WriteRangeBinaryStream<uint32_t>(p, p + blob.size(), data);
      LTRACEIF(!success, ("%s: failed to write glyph bitmap blob.", rawNameExt));
    }
  }

  if (success)
  {
    success = WriteBinaryStream(shaderHash, data);
    LTRACEIF(!success, ("%s: Failed to write shader hash.", rawNameExt));
  }
  return success;
}

}
#endif

//==============================================================================
bool Font::OnLoaded(Buffer buffer)
{
  BufferReader reader(buffer);
  bool success = reader.Read(m_lineHeight) &&
    m_lineHeight > .0f &&
    reader.Read(m_ascent) &&
    m_ascent > .0f &&
    reader.Read(m_cacheSideSizePixels) &&
    m_cacheSideSizePixels > 0;
  LTRACEIF(!success, ("%s: failed to read font metrics.", m_debugPath.c_str()));

  // num glyphs
  uint32_t numGlyphs = 0;
  if(success)
  {
    success = reader.Read(numGlyphs);
    LTRACEIF(!success, ("%s: failed to read glyph count.", m_debugPath.c_str()));
  }

  uint32_t maxHeight = 0;
  if (success)
  {
    Glyph g;
    uint32_t i = 0;
    while (i < numGlyphs)
    {
      success = reader.Read(g);
      if (success)
      {
        auto h = g.fieldHeight;
        if (h > maxHeight)
        {
          maxHeight = h;
        }
        m_glyphs[g.codePoint] = g;
        ++i;
      }
      else
      {
        LTRACE(("%s: failed to read glyph %d.", m_debugPath.c_str(), i));
        break;
      }
    }
  }

  uint32_t glyphBitmapsSize;
  if(success)
  {
    success = reader.Read(glyphBitmapsSize);
    LTRACEIF(!success, ("%s: failed to read bitmap size.", m_debugPath.c_str()));
  }

  uint32_t readSize;
  if (success)
  {
    success = reader.Read(readSize) && readSize <= reader.GetRemainingSize();
    LTRACEIF(!success,
      ("%s: unexpected end of buffer before bitmap blob (%d bytes missing).",
      m_debugPath.c_str(), readSize - reader.GetRemainingSize()));
  }

  if(success)
  {
    m_glyphBitmaps.resize(glyphBitmapsSize);

    auto writep = m_glyphBitmaps.data();
    auto writeEnd = m_glyphBitmaps.data() + m_glyphBitmaps.size();
    while (writep != writeEnd)
    {
      auto readp = reader.ReadBytesWithSize(readSize);
      success = readp != nullptr;
      if (success)
      {
        memcpy(writep, readp, readSize);
        writep += readSize;
      }
      else
      {
        XR_TRACE(Font,
          ("%s: failed to read %z bytes of bitmap data (read %z so far).",
            m_debugPath.c_str(), readSize, writep - m_glyphBitmaps.data()));
        break;
      }
    }
  }

  if (success)
  {
    uint32_t flags = GetFlags() | UnmanagedFlag;  // shouldn't need explicit unmanage.
    m_texture.Reset(Texture::Create(0, flags)->Cast<Texture>());

    m_textureCache = new TextureCache(m_cacheSideSizePixels,
      TextureCache::Format::U8, 8, maxHeight);
    m_cachedGlyphs.reserve(128);

    m_cacheDirty = true; // We do want a texture update even if no glyphs were cached.
  }

  return success;
}

//==============================================================================
void Font::OnUnload()
{
  m_glyphs.clear();
  std::vector<uint8_t>().swap(m_glyphBitmaps);

  std::vector<CachedGlyphInternal>().swap(m_cachedGlyphs);
  delete m_textureCache;
  m_textureCache = nullptr;
  m_cacheDirty = false;

  m_texture.Reset(nullptr);
}

//==============================================================================
Font::CachedGlyph const* Font::CacheGlyph(uint32_t codePoint)
{
  CachedGlyph const* cg = nullptr;
  if (auto cgi = CacheGlyphInternal(codePoint))
  {
    cg = &cgi->glyph;
  }
  return cg;
}

//==============================================================================
void Font::UpdateCache()
{
  XR_ASSERT(Font, m_textureCache);
  if (m_cacheDirty)
  {
    auto size = m_textureCache->GetSize();
    m_texture->Upload(Gfx::TextureFormat::R8, size, size, { size *
      m_textureCache->GetPitch(), m_textureCache->GetBuffer() });
    m_cacheDirty = false;

#ifdef ENABLE_GLYPH_DEBUG
    Image img;
    img.SetPixelData(m_textureCache->GetBuffer(), m_textureCache->GetSize(),
      m_textureCache->GetSize(), 1);

    img.Save("font-cache.tga", true);
#endif
  }
}

//==============================================================================
void Font::ClearCache()
{
  m_cachedGlyphs.clear();
  if(m_textureCache)
  {
    m_textureCache->Reset();
  }
}

//==============================================================================
Texture::Ptr Font::GetTexture() const
{
  return m_texture;
}

//==============================================================================
Font::CachedGlyphInternal* Font::CacheGlyphInternal(uint32_t codePoint)
{
  CachedGlyphInternal* result = nullptr;
  auto iCached = std::lower_bound(m_cachedGlyphs.begin(), m_cachedGlyphs.end(),
    codePoint, [](CachedGlyphInternal const& cg, uint32_t cp) {
    return cg.glyph.glyph->codePoint < cp;
  });
  if (iCached == m_cachedGlyphs.end() || iCached->glyph.glyph->codePoint != codePoint) // not cached
  {
    auto iGlyph = m_glyphs.find(codePoint);
    if (iGlyph != m_glyphs.end())
    {
      Glyph const& glyph = iGlyph->second;
      CachedGlyphInternal cg = { nullptr, { &glyph, AABB() } };
      if (glyph.fieldWidth > 0)
      {
        cg.buffer = m_textureCache->Allocate(glyph.fieldWidth, glyph.fieldHeight,
          cg.glyph.uvs);
        if (cg.buffer)
        {
          // copy the bitmap data to the texture
          auto writep = cg.buffer;
          auto readp = m_glyphBitmaps.data() + glyph.dataOffset;
          auto readEnd = readp + glyph.fieldWidth * glyph.fieldHeight;
          auto pitch = m_textureCache->GetPitch();
          while (readp != readEnd)
          {
            memcpy(writep, readp, glyph.fieldWidth);
            writep += pitch;
            readp += glyph.fieldWidth;
          }

          m_cacheDirty = true;
        }
        else
        {
          LTRACE(("%s: Failed to allocate cache space for glyph 0x%x.",
            m_debugPath.c_str(), codePoint));
        }
      }

      iCached = m_cachedGlyphs.insert(iCached, cg);
      result = &*iCached;
    }
  }
  else
  {
    result = &*iCached;
  }
  return result;
}

} // xr

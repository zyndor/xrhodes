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

#include "XR/TextureCache.hpp"
#include "XR/XonBuildTree.hpp"
#include "XR/FileBuffer.hpp"
#include "XR/Font.hpp"
#include "XR/BufferReader.hpp"
#include "XR/streamutils.hpp"
#include "XR/stringutils.hpp"
#include "XR/utils.hpp"
#include "XR/debug.hpp"
#include "stb_truetype.h"
#include <limits>
#include <iostream>
#include <list>
#include <regex>

#define ENABLE_GLYPH_DEBUG

#ifdef ENABLE_GLYPH_DEBUG
#include "XR/Image.hpp"
#endif

namespace XR
{

//==============================================================================
XR_ASSET_DEF(Font, "xfnt", 1, "fnt")

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
    XR_TRACE(Font, ("Invalid code point literal: '%s'", value.c_str()));
  }
  return success;
}

bool ParseHexCodePoint(std::string const& value, uint32_t& c)
{
  bool success = !!(std::istringstream(value) >> std::ws >> std::hex >> c);
  if(!success)
  {
    XR_TRACE(Font, ("Failed to parse hex code point value: %s", value.c_str()));
  }
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
  auto root = XonBuildTree(buffer.As<char const>(), buffer.size);
  bool success = root != nullptr;

  // Get font file path, read file.
  FileBuffer ttfData;
  if(success)
  {
    try
    {
      auto& fontPath = root->Find("font");
      success = ttfData.Open(fontPath.GetValue(), false);
    }
    catch(XonEntity::Exception&)
    {
      success = false;
      XR_TRACE(Font, ("%s: missing definition for '%s'", rawNameExt, "font"));
    }
  }

  // Get code point ranges.
  std::unique_ptr<RangeTree> codePointsRoot;
  if(success)
  {
    try
    {
      auto& codePoints = root->Find("codePoints");
      for(size_t i0 = 0, i1 = codePoints.GetNumElements(); i0 < i1; ++i0)
      {
        try
        {
          char const* rangeDef = codePoints[i0].GetValue();
          if(!ParseRange(rangeDef, codePointsRoot))
          {
            XR_TRACE(Font, ("%s: '%s' is not a valid code point range.", rawNameExt, rangeDef));
          }
        }
        catch(XonEntity::Exception&)
        {
          // ignore
        }
      }
    }
    catch(XonEntity::Exception&)
    {
      success = false;
      XR_TRACE(Font, ("%s: missing definition for '%s'", rawNameExt, "codePoints"));
    }
  }

  // Handle some optional attributes.
  int numFonts = 0;
  if (success)
  {
    numFonts = stbtt_GetNumberOfFonts(ttfData.GetData());
    success = numFonts > 0;
    if (!success)
    {
      XR_TRACE(Font, ("%s: Failed to get number of fonts from (%d).",
        rawNameExt, numFonts));
    }
  }

  int fontIndex = 0;
  int fontSize = kDefaultFontSize; // includes padding for SDF, see below.
  int sdfSize = kDefaultSdfSize;
  int cacheSize = kDefaultCacheSize;
  Asset::HashType shaderHash = 0;
  if(success)
  {
    try
    {
      auto& index = root->Find("fontIndex");
      if (!StringTo(index.GetValue(), fontIndex) || fontIndex < 0 ||
        fontIndex >= numFonts)
      {
        fontIndex = 0;
        XR_TRACE(Font, ("%s: '%s' is invalid for %s; defaulting to %d.",
          rawNameExt, index.GetValue(), "fontIndex", fontIndex));
      }
    }
    catch (XonEntity::Exception&)
    {
      // ignore.
    }

    try
    {
      auto& size = root->Find("fontSize");
      if (!StringTo(size.GetValue(), fontSize))
      {
        XR_TRACE(Font, ("%s: Failed to parse %s for %s, defaulting to %d.",
          rawNameExt, size.GetValue(), "fontSize", fontSize));
      }
      else if (fontSize < kMinFontSize)
      {
        XR_TRACE(Font, ("%s: Clamped %s (%d) to minimum (%d).", "fontSize",
          rawNameExt, fontSize, kMinFontSize));
        fontSize = kMinFontSize;
      }
    }
    catch (XonEntity::Exception&)
    {
      // ignore.
    }

    try
    {
      auto& size = root->Find("sdfSize");
      if (!StringTo(size.GetValue(), sdfSize))
      {
        XR_TRACE(Font, ("%s: Failed to parse %s for %s, defaulting to %d.",
          rawNameExt, size.GetValue(), "sdfSize", sdfSize));
      }
      else if (sdfSize < kMinSdfSize)
      {
        XR_TRACE(Font, ("%s: Clamped %s (%d) to minimum (%d).", "sdfSize",
          rawNameExt, sdfSize, kMinSdfSize));
        sdfSize = kMinSdfSize;
      }
      else
      {
        const int maxSdfSize = (fontSize / 2) - 1;
        if (sdfSize > maxSdfSize)
        {
          XR_TRACE(Font, ("%s: Clamped %s (%d) to maximum (%d).", "sdfSize",
            rawNameExt, sdfSize, maxSdfSize));
          sdfSize = maxSdfSize;
        }
      }
    }
    catch (XonEntity::Exception&)
    {
      // ignore.
    }

    try
    {
      auto& size = root->Find("cacheSize");
      if (!StringTo(size.GetValue(), cacheSize))
      {
        XR_TRACE(Font, ("%s: Failed to parse %s for %s, defaulting to %d.",
          rawNameExt, size.GetValue(), "cacheSize", cacheSize));
      }
      else if (cacheSize < kMinCacheSize)
      {
        XR_TRACE(Font, ("%s: Clamped %s (%d) to minimum (%d).", "cacheSize",
          rawNameExt, cacheSize, kMinCacheSize));
        cacheSize = std::min(cacheSize, kMinCacheSize);
      }
    }
    catch (XonEntity::Exception&)
    {
      // ignore.
    }

    try
    {
      auto& shader = root->Find("shader");
      if (auto shaderPath = shader.GetValue())
      {
        dependencies.push_back(shaderPath);
        shaderHash = Asset::Manager::HashPath(dependencies.back());
      }
    }
    catch (XonEntity::Exception&)
    {
      // ignore.
    }
  }

  // Init font data.
  stbtt_fontinfo stbFont;
  if(success)
  {
    int offset = stbtt_GetFontOffsetForIndex(ttfData.GetData(), fontIndex);
    success = stbtt_InitFont(&stbFont, ttfData.GetData(), offset) != 0;
    if (!success)
    {
      XR_TRACE(Font, ("%s: Failed to initialise font.", rawNameExt));
    }
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
    if (!success)
    {
      XR_TRACE(Font, ("%s: Failed to write font metrics.", rawNameExt));
    }
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
      XR_TRACE(Font, ("%s: Failed to write number of glyphs.", rawNameExt));
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
          XR_TRACE(Font, ("%s: No glyph found for code point 0x%x.", rawNameExt, i0));
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

#ifdef ENABLE_GLYPH_DEBUG
          //XR_TRACE(Font, ("0x%x (%c) Bitmap @ %d x %d (+ %d, %d)", i0, i0, w, h, xPixelOffs, yPixelOffs));
          //{
          //  auto pp = glyphBitmapPadded + bufferOffset;
          //  auto rowDiff = glyphSizePadded - w;
          //  for(int yy = 0; yy < h; ++yy)
          //  {
          //    for(int xx = 0; xx < w; ++xx)
          //    {
          //      printf("%x", *pp / 16);
          //      ++pp;
          //    }
          //    printf("\n");
          //    pp += rowDiff;
          //  }
          //}
#endif

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

#ifdef ENABLE_GLYPH_DEBUG
          //XR_TRACE(Font, ("0x%x (%c) SDF Bitmap @ %d x %d", i0, i0, sw, sh));
          //{
          //  auto pp = glyphBitmap.data();
          //  for (int yy = 0; yy < sh; ++yy)
          //  {
          //    for (int xx = 0; xx < sw; ++xx)
          //    {
          //      printf("%x", *pp / 16);
          //      ++pp;
          //    }
          //    printf("\n");
          //  }
          //}
#endif

          glyph.fieldWidth = sw;
          glyph.fieldHeight = sh;
          XR_ASSERT(Font, glyphBytesWritten < std::numeric_limits<decltype(glyph.dataOffset)>::max());
          glyph.dataOffset = static_cast<uint32_t>(glyphBytesWritten);

          // write glyph bitmap data to other stream.
          auto p = glyphBitmap.data();
#ifdef ENABLE_GLYPH_DEBUG
          Image img;
          img.SetPixelData(p, w, h, 1);

          XR::FilePath path;
          sprintf(path.data(), "glyph-%04x.tga", i0);
          path.UpdateSize();
          img.Save(path, true);
#endif

          if (!WriteRangeBinaryStream<uint32_t>(p, p + (sw * sh), glyphBitmaps,
            &glyphBytesWritten))
          {
            success = false;
            XR_TRACE(Font, ("%s: Failed to write glyph bitmap data for 0x%x in '%s'.",
              rawNameExt, i0));
            break;
          }
        }

        // write glyph.
        if(!WriteBinaryStream(glyph, data))
        {
          success = false;
          XR_TRACE(Font, ("%s: Failed to write glyph data for 0x%x.", rawNameExt, i0));
          break;
        }
      }

      iRanges = iRanges->Higher();
    }

    if(success && !WriteBinaryStream(static_cast<uint32_t>(glyphBytesWritten), data))
    {
      success = false;
      XR_TRACE(Font, ("%s: Failed to write glyp bitmap blob size for.", rawNameExt));
    }

    if(success)
    {
      auto blob = glyphBitmaps.str();
      auto p = reinterpret_cast<uint8_t const*>(blob.data());
      success = WriteRangeBinaryStream<uint32_t>(p, p + blob.size(), data);
      XR_TRACEIF(Font, !success, ("%s: Failed to write glyph bitmap blob for '%s'.", rawNameExt));
    }
  }

  if (success)
  {
    success = WriteBinaryStream(shaderHash, data);
    XR_TRACEIF(Font, !success, ("%s: Failed to write shader for", rawNameExt));
  }
  return success;
}

}
#endif

//==============================================================================
Asset::Descriptor<Shader> Font::s_descDefaultShader;

bool Font::OnLoaded(Buffer buffer)
{
  BufferReader reader(buffer);
  bool success = reader.Read(m_lineHeight) &&
    m_lineHeight > .0f &&
    reader.Read(m_ascent) &&
    m_ascent > .0f &&
    reader.Read(m_cacheSideSizePixels) &&
    m_cacheSideSizePixels > 0;

  // num glyphs
  uint32_t numGlyphs = 0;
  if(success)
  {
    success = reader.Read(numGlyphs);
  }

  uint32_t maxHeight = 0;
  Glyph g;
  uint32_t i = 0;
  while(success && i < numGlyphs)
  {
    success = reader.Read(g);
    auto h = g.fieldHeight;
    if(h > maxHeight)
    {
      maxHeight = h;
    }
    m_glyphs[g.codePoint] = g;
    ++i;
  }

  uint32_t glyphBitmapsSize;
  if(success)
  {
    success = reader.Read(glyphBitmapsSize) &&
      glyphBitmapsSize <= reader.GetRemainingSize();
  }

  if(success)
  {
    m_glyphBitmaps.resize(glyphBitmapsSize);

    uint32_t readSize;
    success = reader.Read(readSize) && readSize <= reader.GetRemainingSize();

    auto writep = m_glyphBitmaps.data();
    auto writeEnd = m_glyphBitmaps.data() + m_glyphBitmaps.size();
    while (success && writep != writeEnd)
    {
      auto readp = reader.ReadBytesWithSize(readSize);
      success = readp != nullptr;
      if (success)
      {
        memcpy(writep, readp, readSize);
        writep += readSize;
      }
    }
  }

  Descriptor<Shader> descShader;
  if (success)
  {
    success = reader.Read(descShader.hash);
  }

  Shader::Ptr shader;
  if (success)
  {
    bool useDefault = !descShader.IsValid() && s_descDefaultShader.IsValid();
    if (useDefault)
    {
      descShader = s_descDefaultShader;
    }

    success = descShader.IsValid();
    if (success)
    {
      shader = Manager::Find(descShader);
      success = shader != nullptr;
      if (!success)
      {
        XR_TRACE(Font, ("%s: failed to set shader 0x%llx%s.", m_debugPath.c_str(),
          descShader.hash, useDefault ? " (bad default shader!)": ""));
      }
    }
    else
    {
      XR_TRACE(Font, ("%s: missing shader definition and no default shader."));
    }

    if (success && !s_descDefaultShader.IsValid())
    {
      s_descDefaultShader = descShader;
      XR_TRACE(Font, ("%s: setting default shader to 0x%llx.", m_debugPath.c_str(),
        descShader.hash));
    }
  }

  if (success)
  {
    uint32_t flags = GetFlags() | UnmanagedFlag;  // shouldn't need explicit unmanage.
    m_material.Reset(Material::Create(0, flags)->Cast<Material>());
    m_material->OverrideStateFlags(0, Gfx::F_STATE_ALPHA_BLEND);
    m_material->SetShader(shader);
    m_material->SetTexture(0, Texture::Create(0, flags)->Cast<Texture>());

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

  m_material.Reset(nullptr);
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
void Font::ClearCache()
{
  m_cachedGlyphs.clear();
  if(m_textureCache)
  {
    m_textureCache->Reset();
  }
}

//==============================================================================
Material::Ptr Font::GetMaterial()
{
  XR_ASSERT(Font, m_textureCache);
  if (m_cacheDirty)
  {
    Texture::Ptr texture = m_material->GetTexture(0);
    auto size = m_textureCache->GetSize();
    texture->Upload(Gfx::TextureFormat::R8, size, size, { size *
      m_textureCache->GetPitch(), m_textureCache->GetBuffer() });
    m_cacheDirty = false;

#ifdef ENABLE_GLYPH_DEBUG
    Image img;
    img.SetPixelData(m_textureCache->GetBuffer(), m_textureCache->GetSize(),
      m_textureCache->GetSize(), 1);

    img.Save("font-cache.tga", true);
#endif
  }

  return m_material;
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
          XR_TRACE(Font, ("Failed to allocate cache space for glyph 0x%x.", codePoint));
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

} // XR

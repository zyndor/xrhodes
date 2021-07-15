//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "IsSpaceUtf8.hpp"
#include "xr/TextureCache.hpp"
#include "xr/Font.hpp"
#include "xr/memory/BufferReader.hpp"
#ifdef ENABLE_ASSET_BUILDING
#include "SdfBuilder.hpp"
#include "xr/xon/XonBuildTree.hpp"
#include "xr/FileBuffer.hpp"
#include "xr/io/streamutils.hpp"
#include "xr/strings/stringutils.hpp"
#include "xr/utils.hpp"
#include "xr/debug.hpp"
#include "stb_truetype.h"
#include <limits>
#include <iostream>
#include <list>
#include <regex>
#include <locale>
#endif

//#define ENABLE_GLYPH_DEBUG

#ifdef ENABLE_GLYPH_DEBUG
#include "xr/Image.hpp"
#endif

#define LTRACE(format) XR_TRACE(Font, format)
#define LTRACEIF(condition, format) XR_TRACEIF(Font, condition, format)

namespace xr
{

//==============================================================================
XR_ASSET_DEF(Font, "xfnt", 3, "fnt")

/*
  {
    font: "ComicSans.ttf",
    fontSize: 128,
    fontIndex: 0,
    sdfSize: 4,
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

namespace {

#ifdef ENABLE_GLYPH_DEBUG
char const* const kAsciiPalette = ".,:;cijlkdG608$@";

void VisualizeBuffer(uint8_t* p, int32_t w, int32_t h, uint32_t stride)
{
  printf("%d x %d\n", w, h);
  auto rowDiff = stride - w;
  for (int i = 0; i < h; ++i)
  {
    printf("%03d ", i);
    for (int j = 0; j < w; ++j)
    {
      printf("%c", kAsciiPalette[(*p >> 4)]);
      ++p;
    }
    p += rowDiff;
    printf("\n");
  }
}
#endif

#ifdef ENABLE_ASSET_BUILDING
struct Range
{
  uint32_t start;
  uint32_t end;
};

const int32_t kMinFontSize = 32;
const int32_t kDefaultFontSize = 128;

const int32_t kDefaultSdfSize = 0;

const int32_t kMinCacheSize = 256;
const int32_t kDefaultCacheSize = 1024;

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

bool ParseRange(char const* def, Range& range)
{
  std::cmatch results;
  bool success = std::regex_match(def, results, kValueRegex);
  if (success)
  {
    std::string match = results[2].str();
    if (!match.empty())
    {
      success = ParseLiteralChar(match, range.start);
    }
    else
    {
      success = ParseHexCodePoint(results[3].str(), range.start); // results[3].str() cannot be empty, since it's matched the regex.
    }

    if(success)
    {
      match = results[6].str();
      if (!match.empty())
      {
        success = ParseLiteralChar(match, range.end);
      }
      else
      {
        match = results[7].str();
        if(!match.empty())
        {
          success = ParseHexCodePoint(match, range.end);
        }
        else
        {
          range.end = range.start;
        }
      }
    }

    if (success)
    {
      if (range.end < range.start)
      {
        std::swap(range.start, range.end);
      }

      ++range.end;
      success = range.end > range.start;
    }
  }
  return success;
}

void MergeRange(Range range, std::vector<Range>& ranges)
{
  XR_ASSERT(Font, range.end >= range.start);
  auto i0 = std::lower_bound(ranges.begin(), ranges.end(), range,
    [](Range const& r0, Range const& r1) {
      return r0.start < r1.start;
    });
  if (i0 != ranges.end() && range.end >= i0->start)
  {
    i0->start = std::min(i0->start, range.start);

    auto i1 = std::upper_bound(i0, ranges.end(), range,
      [](Range const& r0, Range const& r1) {
        return r0.end < r1.end;
      });
    if (i1 != ranges.end())
    {
      range.end = std::max(range.end, i1->end);
    }
    i0->end = std::max(i0->end, range.end);

    ++i0;
    ranges.erase(i0, i1);
  }
  else
  {
    i0 = ranges.insert(i0, range);
  }
}

void SplitRanges(uint32_t value, std::vector<Range>& ranges)
{
  auto i0 = std::upper_bound(ranges.begin(), ranges.end(), value,
    [](uint32_t v, Range const& range) {
      return v < range.end;
    });
  if (i0 != ranges.end())
  {
    if (i0->start == value)
    {
      ++i0->start;
    }
    else
    {
      const uint32_t end = i0->end;
      i0->end = value;

      ++value;
      if (value < end)
      {
        ++i0;
        ranges.insert(i0, Range{ value, end });
      }
    }
  }
}

XR_ASSET_BUILDER_DECL(Font)

XR_ASSET_BUILDER_BUILD_SIG(Font)
{
  XonParser::State state;
  std::unique_ptr<XonObject> root(XonBuildTree(buffer.As<char const>(), buffer.size, &state));
  if (!root)
  {
    LTRACE(("%s: failed to parse XON somewhere around row %d, column %d.",
        rawNameExt, state.row, state.column));
    return false;
  }

  // Get font file path, read file.
  FileBuffer ttfData;
  try
  {
    auto& fontPath = root->Get("font").ToValue();
    if (!ttfData.Open(fontPath.GetString()))
    {
      LTRACE(("%s: failed to load '%s'.", rawNameExt, fontPath.GetString()));
      return false;
    }
  }
  catch(XonEntity::Exception&)
  {
    LTRACE(("%s: missing definition for '%s'", rawNameExt, "font"));
    return false;
  }

  // Handle some optional attributes.
  int32_t numFonts = stbtt_GetNumberOfFonts(ttfData.GetData());
  if (numFonts < 1)
  {
    LTRACE(("%s: failed to get number of fonts (%d).", rawNameExt, numFonts));
    return false;
  }

  int32_t fontIndex = 0;
  int32_t fontSize = kDefaultFontSize; // includes padding for SDF, see below.
  int32_t sdfSize = kDefaultSdfSize;
  int32_t cacheSize = kDefaultCacheSize;
  if (auto xon = root->TryGet("fontIndex"))
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
      LTRACE(("%s: %s has invalid type.", rawNameExt, "fontIndex"));
      return false;
    }
  }

  if (auto xon = root->TryGet("fontSize"))
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
      LTRACE(("%s: %s has invalid type.", rawNameExt, "fontSize"));
      return false;
    }
  }

  if (auto xon = root->TryGet("sdfSize"))
  {
    try
    {
      auto& sizeValue = xon->ToValue();
      if (!StringTo(sizeValue.GetString(), sdfSize))
      {
        LTRACE(("%s: failed to parse %s for %s, defaulting to %d.", rawNameExt,
          sizeValue.GetString(), "sdfSize", sdfSize));
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
        LTRACE(("%s: %s has invalid type.", rawNameExt, "sdfSize"));
        return false;
      }
    }
  }

  if (auto xon = root->TryGet("cacheSize"))
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
      LTRACE(("%s: %s has invalid type.", rawNameExt, "cacheSize"));
      return false;
    }
  }

  // Init font data.
  int offset = stbtt_GetFontOffsetForIndex(ttfData.GetData(), fontIndex);
  stbtt_fontinfo stbFont;
  if (stbtt_InitFont(&stbFont, ttfData.GetData(), offset) == 0)
  {
    LTRACE(("%s: failed to initialise font (offset %d).", rawNameExt, offset));
    return false;
  }

  // Calculate and write some font metrics. These come in 1/64 sub-pixel units.
  // We're writing pixels.
  int32_t ascent, descent, lineGap;
  float sdfSizeUnits;
  int32_t x0, x1, y0, y1;
  float pixelScale; // reciprocal of total line height, i.e. ascent - descent + lineGap.

  stbtt_GetFontVMetrics(&stbFont, &ascent, &descent, &lineGap);
  const int32_t lineHeightUnits = ascent - descent + lineGap;

  stbtt_GetFontBoundingBox(&stbFont, &x0, &y0, &x1, &y1);
  const int32_t heightUnits = y1 - y0;
  sdfSizeUnits = (sdfSize / float(fontSize)) * heightUnits;

  pixelScale = stbtt_ScaleForPixelHeight(&stbFont, float(fontSize - sdfSize * 2));

  if (!(WriteBinaryStream(lineHeightUnits * kUnitsToPixel, data) &&
    WriteBinaryStream(ascent * kUnitsToPixel, data) &&
    WriteBinaryStream(cacheSize, data)))
  {
    LTRACE(("%s: failed to write font metrics.", rawNameExt));
    return false;
  }

  // Process code point ranges.
  std::vector<Range> ranges;
  ranges.reserve(8);
  try
  {
    auto& codePoints = root->Get("codePoints").ToObject();
    Range range;
    for (size_t i0 = 0, i1 = codePoints.GetNumElements(); i0 < i1; ++i0)
    {
      try
      {
        char const* rangeDef = codePoints[i0].ToValue().GetString();
        if (!ParseRange(rangeDef, range))
        {
          LTRACE(("%s: '%s' is not a valid code point range.", rawNameExt,
            rangeDef));
        }
        else
        {
          MergeRange(range, ranges);
        }
      }
      catch (XonEntity::Exception const&)
      {
        LTRACE(("%s: ignoring code point entry %d: should be a value.", rawNameExt, i0));
      }
    }
  }
  catch (XonEntity::Exception const&)
  {
    LTRACE(("%s: missing definition for '%s'", rawNameExt, "codePoints"));
    return false;
  }

  // Calculate and write number of glyphs
  uint32_t numGlyphs = 0;
  std::vector<uint32_t> invalid;
  invalid.reserve(8);
  for (auto& r: ranges)
  {
    numGlyphs += r.end - r.start;
    for (auto i0 = r.start; i0 != r.end; ++i0)
    {
      if (stbtt_FindGlyphIndex(&stbFont, i0) <= 0)  // TODO: cache index?
      {
        LTRACE(("%s: no glyph found for code point 0x%x.", rawNameExt, i0));
        --numGlyphs;
        invalid.push_back(i0);
      }
    }
  }

  for (auto i: invalid)
  {
    SplitRanges(i, ranges);
  }

  if (!WriteBinaryStream(numGlyphs, data))
  {
    LTRACE(("%s: failed to write number of glyphs.", rawNameExt));
    return false;
  }

  // Now calculate SDF from TTF. The reason we're not using stbtt's SDF
  // functionality is that we want to control (and minimize) the allocations.
  const int32_t ascentPixels = int(std::ceil(ascent * pixelScale));

  const int32_t glyphPadding = 1; // We pad the glyph bitmap to simplify comparing neighbour pixels, where we'd need to special case at the edges - reading off by one.
  const int32_t totalPadding = glyphPadding + sdfSize;
  const int32_t maxWidth = int32_t(std::ceil((x1 - x0) * pixelScale));
  const int32_t maxHeight = int32_t(std::ceil((y1 - y0) * pixelScale));
  const int32_t glyphWidthPadded = maxWidth + totalPadding * 2;
  const int32_t glyphHeightPadded = maxHeight + totalPadding * 2;
#ifdef ENABLE_GLYPH_DEBUG
  printf("max: %d x %d\n", glyphWidthPadded, glyphHeightPadded);
#endif

  std::vector<uint8_t> glyphBitmap(glyphWidthPadded * glyphHeightPadded);
  uint8_t* glyphBitmapPadded = glyphBitmap.data() + glyphWidthPadded + glyphPadding;

  std::unique_ptr<SdfBuilder> sdf;
  if (sdfSize > 0)
  {
    sdf.reset(new SdfBuilder(fontSize, sdfSize));
  }

  Font::Glyph glyph;
  std::ostringstream glyphBitmaps;
  size_t glyphBytesWritten = 0;
  for (auto& r: ranges)
  {
    for(auto i0 = r.start, i1 = r.end; i0 != i1; ++i0)
    {
      auto const iGlyph = stbtt_FindGlyphIndex(&stbFont, i0);
      XR_ASSERT(Font, iGlyph > 0);

      glyph.codePoint = i0;

      // Get glyph metrics.
      int32_t advance;
      int32_t xBearing;
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

      if (IsSpaceUtf8(i0)) // if whitespace, don't waste memory.
      {
        glyph.fieldHeight = 0;
        glyph.fieldWidth = 0;
        glyph.dataOffset = uint32_t(-1);
      }
      else
      {
        // Copy glyph bitmap data to an area padded for SDF.
        std::fill(glyphBitmap.begin(), glyphBitmap.end(), uint8_t(0x00));

        // Get glyph bitmap box coordinates.
        // Y increases downwards and the origin is the baseline / ascent.
        // Y0 is top (ascent + y0 for absolute coords), Y1 is bottom.
        stbtt_GetGlyphBitmapBox(&stbFont, iGlyph, pixelScale, pixelScale,
          &x0, &y0, &x1, &y1);
        int32_t w = x1 - x0;
        int32_t h = y1 - y0;

        if (sdf)
        {
          // Blit the glyph at its absolute position (sdf padding and ascent applies).
          auto xBearingPixels = int32_t(std::ceil(glyph.xBearing * pixelScale));
          auto xPixelOffs = std::max(0, sdfSize + xBearingPixels);
          auto yPixelOffs = std::max(0, sdfSize + ascentPixels + y0);
          auto bufferOffset = xPixelOffs + yPixelOffs * glyphWidthPadded;

          stbtt_MakeGlyphBitmap(&stbFont, glyphBitmapPadded + bufferOffset,
            w, h, glyphWidthPadded, pixelScale, pixelScale, iGlyph);

#ifdef ENABLE_GLYPH_DEBUG
          printf("%04x: ", i0);
          VisualizeBuffer(glyphBitmapPadded + bufferOffset, w, h, glyphWidthPadded);
#endif

          // Calculate SDF metrics & generate SDF around glyph.
          auto sx0 = std::max(xPixelOffs - sdfSize, 0);
          auto sy0 = std::max(yPixelOffs - sdfSize, 0);  // ascentPixels + y0; never really < 0.
          auto sx1 = std::min(xPixelOffs + w + sdfSize, fontSize);
          auto sy1 = std::min(yPixelOffs + h + sdfSize, fontSize);
          w = sx1 - sx0;
          h = sy1 - sy0;
          bufferOffset = sx0 + sy0 * glyphWidthPadded;
          sdf->Generate(glyphBitmapPadded + bufferOffset, glyphWidthPadded, w, h);
          sdf->ConvertToBitmap(w, h, glyphBitmap.data());
        }
        else
        {
          // Blit the glyph at its absolute position (ascent applies).
          stbtt_MakeGlyphBitmap(&stbFont, glyphBitmap.data(), w, h, w, pixelScale,
            pixelScale, iGlyph);
        }

        glyph.fieldWidth = w;
        glyph.fieldHeight = h;
        XR_ASSERT(Font, glyphBytesWritten < std::numeric_limits<decltype(glyph.dataOffset)>::max());
        glyph.dataOffset = static_cast<uint32_t>(glyphBytesWritten);

        auto p = glyphBitmap.data();
#ifdef ENABLE_GLYPH_DEBUG
        printf("%s%04x: ", sdf ? "SDF " : "", i0);
        VisualizeBuffer(p, w, h, w);
#endif

        // write glyph bitmap data to other stream.
        if (!WriteRangeBinaryStream<uint32_t>(p, p + (w * h), glyphBitmaps,
          &glyphBytesWritten))
        {
          LTRACE(("%s: failed to write glyph bitmap data for 0x%x.", rawNameExt, i0));
          return false;
        }
      }

      // write glyph.
      if(!WriteBinaryStream(glyph, data))
      {
        LTRACE(("%s: failed to write glyph data for 0x%x.", rawNameExt, i0));
        return false;
      }
    }
  }

  if(!WriteBinaryStream(static_cast<uint32_t>(glyphBytesWritten), data))
  {
    LTRACE(("%s: failed to write glyp bitmap blob size.", rawNameExt));
    return false;
  }

  auto blob = glyphBitmaps.str();
  auto p = reinterpret_cast<uint8_t const*>(blob.data());
  if (!WriteRangeBinaryStream<uint32_t>(p, p + blob.size(), data))
  {
    LTRACE(("%s: failed to write glyph bitmap blob.", rawNameExt));
    return false;
  }

  return true;
}

#endif  // ENABLE_ASSET_BUILDING
}

//==============================================================================
bool Font::OnLoaded(Buffer buffer)
{
  BufferReader reader(buffer);
  if (!(reader.Read(m_lineHeight) &&
    m_lineHeight > .0f &&
    reader.Read(m_ascent) &&
    m_ascent > .0f &&
    reader.Read(m_cacheSideSizePixels) &&
    m_cacheSideSizePixels > 0))
  {
    LTRACE(("%s: failed to read font metrics.", m_debugPath.c_str()));
    return false;
  }

  // num glyphs
  uint32_t numGlyphs = 0;
  if(!reader.Read(numGlyphs))
  {
    LTRACE(("%s: failed to read glyph count.", m_debugPath.c_str()));
    return false;
  }

  uint32_t maxHeight = 0;
  Glyph g;
  uint32_t i = 0;
  while (i < numGlyphs)
  {
    if (reader.Read(g))
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
      return false;
    }
  }

  uint32_t glyphBitmapsSize;
  if(!reader.Read(glyphBitmapsSize))
  {
    LTRACE(("%s: failed to read bitmap size.", m_debugPath.c_str()));
    return false;
  }

  uint32_t readSize;
  if (!(reader.Read(readSize) && readSize <= reader.GetRemainingSize()))
  {
    LTRACE(("%s: unexpected end of buffer before bitmap blob (%d bytes missing).",
      m_debugPath.c_str(), readSize - reader.GetRemainingSize()));
    return false;
  }

  m_glyphBitmaps.resize(glyphBitmapsSize);

  auto writep = m_glyphBitmaps.data();
  auto writeEnd = m_glyphBitmaps.data() + m_glyphBitmaps.size();
  while (writep != writeEnd)
  {
    auto readp = reader.ReadBytesWithSize(readSize);
    if (readp != nullptr)
    {
      memcpy(writep, readp, readSize);
      writep += readSize;
    }
    else
    {
      LTRACE(("%s: failed to read %z bytes of bitmap data (read %z so far).",
        m_debugPath.c_str(), readSize, writep - m_glyphBitmaps.data()));
      return false;
    }
  }

  uint32_t flags = GetFlags() | UnmanagedFlag;  // shouldn't need explicit unmanage.
  m_texture.Reset(Texture::Create(0, flags));

  m_textureCache = new TextureCache(m_cacheSideSizePixels,
    TextureCache::Format::U8, 8, maxHeight);
  m_cachedGlyphs.reserve(128);

  m_cacheDirty = true; // We do want a texture update even if no glyphs were cached.

  return true;
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
    Buffer buffer = { size * m_textureCache->GetPitch(), m_textureCache->GetBuffer() };
    m_texture->Upload(Gfx::TextureFormat::R8, size, size, Gfx::F_TEXTURE_NONE,
      1, &buffer);
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

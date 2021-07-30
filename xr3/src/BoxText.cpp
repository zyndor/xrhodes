//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "IsSpaceUtf8.hpp"
#include "xr/BoxText.hpp"
#include "xr/Sprite.hpp"
#include "xr/IndexMesh.hpp"
#include "xr/meshutil.hpp"
#include "utf8/unchecked.h"

namespace utf8u = utf8::unchecked;

namespace xr {

//==============================================================================
BoxText::BoxText()
: m_scale(1.f),
  m_boxSize(std::numeric_limits<float>::max(), 0.f),
  m_horizontalAlignment(Alignment::Centered),
  m_verticalAlignment(Alignment::Centered),
  m_origin(0.f, 0.f),
  m_horizontalSpacing(0.f),
  m_verticalSpacing(0.f)
{}

//==============================================================================
BoxText::~BoxText()
{}

//==============================================================================
void BoxText::SetFont(const Font::Ptr& font)
{
  m_font = font;
}

//==============================================================================
void BoxText::SetScale(float scale)
{
  m_scale = scale;
}

//==============================================================================
void  BoxText::SetBoxSize(float width, float height)
{
  XR_ASSERT(BoxText, width >= 0.f);
  XR_ASSERT(BoxText, height >= 0.f);

  m_boxSize = Vector2(width, height);
}

//==============================================================================
void BoxText::SetHorizontalAlignment(Alignment ha)
{
  m_horizontalAlignment = ha;
}

//==============================================================================
void BoxText::SetVerticalAlignment(Alignment va)
{
  m_verticalAlignment = va;
}

//==============================================================================
void BoxText::SetHorizontalSpacing(float hs)
{
  m_horizontalSpacing = hs;
}

//==============================================================================
void BoxText::SetVerticalSpacing(float vs)
{
  m_verticalSpacing = vs;
}

//==============================================================================
void BoxText::SetOrigin(Vector2 origin)
{
  m_origin = origin;
}

//==============================================================================
void BoxText::EnsureVerticalFit(uint32_t numLines)
{
  XR_ASSERT(BoxText, m_font);
  float requiredHeight = m_font->GetLineHeight() * m_scale * numLines +
    m_verticalSpacing * m_scale * (numLines > 0)  * (numLines - 1);
  if (m_boxSize.y < requiredHeight)
  {
    m_boxSize.y = requiredHeight;
  }
}

//==============================================================================
void BoxText::Measure(char const* text, Measurement& m) const
{
  auto numBytes = strlen(text);
  Measure(text, numBytes, m);
}

//==============================================================================
void BoxText::Measure(char const* text, size_t numBytes, Measurement& m) const
{
  const Font& font = *m_font;

  const float missingGlyphAdvance = 0.f;
  const float missingGlyphBearing = 0.f;
  const float lineHeight = font.GetLineHeight();

  // convert boxSize into font units, since that's what glyph metrics are stored in.
  const Vector2 boxSize = m_boxSize / m_scale;
  if (lineHeight > boxSize.y) // can't fit a single line
  {
    m = Measurement{ Vector2::Zero(), 0, {} };
    return;
  }

  const float hSpacing = m_horizontalSpacing / m_scale;
  const float vSpacing = m_verticalSpacing / m_scale;

  float position = 0.f;  // of current row, world units
  float widthLineStart = 0.f;
  float widthWordEnd = 0.f; // width at the end of the last completed word, world units
  float widthWordStart = 0.f; // width at the start of the current word, world units
  char const* wordStart = nullptr;
  char const* wordEnd = nullptr;
  char const* nextLineStart = nullptr;

  Measurement tm{ Vector2::Zero(), 0, {} };
  tm.lines.reserve(size_t(std::floor(boxSize.y / (lineHeight + vSpacing))));

  Measurement::Line line{ nullptr, nullptr, 0.f };

  auto endText = utf8::find_invalid(text, text + numBytes);
  while (text != endText)
  {
    const auto cp = utf8u::peek_next(text);
    const bool isCr = cp == '\r';
    if (isCr || cp == '\n')  // line break
    {
      line.end = text;
      line.width = position - widthLineStart;

      wordStart = nullptr;

      utf8u::advance(text, 1);  // skip newline
      if (isCr && text != endText && utf8u::peek_next(text) == '\n')  // if CR/LF, keep going
      {
        utf8u::advance(text, 1);
      }

      nextLineStart = text;
      position = 0.f;
    }
    else
    {
      // Get glyph.
      auto glyph = font.GetGlyph(cp);
      float glyphAdvance = missingGlyphAdvance;
      float glyphBearing = missingGlyphBearing;
      if (glyph)
      {
        glyphAdvance = glyph->advance;
        glyphBearing = glyph->xBearing;
        if (glyph->fieldHeight != 0)
        {
          ++tm.numGlyphs;
        }
      }

      // If glyph is wider than row, terminate processing.
      if (glyphAdvance > boxSize.x)
      {
        wordStart = nullptr;
        break;
      }

      // Check word boundary and line start.
      const bool isSpace = IsSpaceUtf8(cp);
      if ((wordStart != nullptr) == isSpace) // If we have a word started, and hit space, or if have no word started and hit non-whitespace.
      {
        if (wordStart)  // Had word, now finished.
        {
          widthWordEnd = position;
          wordEnd = text;
          wordStart = nullptr;
        }
        else
        {
          widthWordStart = position;
          wordStart = text;
          if (!line.start)
          {
            line.start = text;
            widthLineStart = position;
            glyphAdvance -= glyphBearing;
          }
        }
      }

      if (position + glyphAdvance > boxSize.x) // We've reached the end of the row.
      {
        if (line.start && wordEnd > line.start) // There was at least one complete word.
        {
          line.end = wordEnd;
          line.width = widthWordEnd - widthLineStart;
          if (wordStart)
          {
            position = position - widthWordStart + glyphAdvance + hSpacing;
          }
          else
          {
            position = 0.f; // discarding space
          }
          nextLineStart = wordStart;
        }
        else // We're either amidst whitespace, or in a word that has spanned the whole row -- we have to linebreak.
        {
          line.end = text;
          line.width = position - widthLineStart;
          if (isSpace)
          {
            wordStart = nullptr;
            position = 0.f;
          }
          else  // in a word; just break on the character and signal new line start.
          {
            wordStart = text;
            position = glyphAdvance - glyphBearing + hSpacing;

            widthLineStart = 0.f;
          }
          nextLineStart = wordStart;
        }
      }
      else
      {
        position += glyphAdvance + hSpacing;
      }

      utf8u::advance(text, 1);
    }

    // Process new line
    if (line.end)
    {
      XR_ASSERT(BoxText, line.start && line.end);
      auto lineChars = utf8u::distance(line.start, line.end);
      line.width -= hSpacing * (lineChars > 0);
      line.width *= m_scale;  // Convert to scaled units.

      tm.size.x = std::max(line.width, tm.size.x);
      tm.lines.push_back(line);

      tm.size.y += lineHeight + vSpacing;
      if (tm.size.y + lineHeight > boxSize.y)  // Prevent vertical overflow.
      {
        break;
      }

      // new line starts here
      line.start = nextLineStart;
      line.end = nullptr;
      // NOTE: nextLineStart = nullptr; not required - we only read it on the previous line and end-of-line scenarios are supposed to set it.
    }
  }

  // Process last row - if there's enough vertical space and row not empty.
  if (text == endText && line.start && tm.size.y + lineHeight <= boxSize.y)
  {
    line.end = text;

    auto lineChars = utf8u::distance(line.start, line.end);
    line.width = position - widthLineStart - hSpacing * (lineChars > 0);
    line.width *= m_scale;

    tm.size.x = std::max(line.width, tm.size.x);
    tm.size.y += lineHeight;
    tm.lines.push_back(line);
  }

  // TODO: ellipses for truncated text?

  // Convert to scaled units now.
  tm.size.y *= m_scale;

  m = std::move(tm);
}

//==============================================================================
namespace
{

using Aligner = void(*)(float, float&);

const Aligner kHorizontalAligners[] = {
  [](float /*width*/, float& /*x*/) {},
  [](float width, float& x) { x += width * .5f; },
  [](float width, float& x) { x += width; },
};

}

void BoxText::Generate(Measurement const& m, uint32_t attribStride,
  Vector3* positions, Vector2* uvs, bool updateGlyphCache, Stats* statsOut) const
{
  Font& font = *m_font;

  const float lineHeight = font.GetLineHeight() * m_scale;
  float maxLineWidth = 0.f;
  float height = m.size.y;

  XR_DEBUG_ONLY(auto posEnd = XR_ADVANCE_PTR(positions, attribStride * m.numGlyphs *
    Quad::Vertex::kCount);)
  if (m.numGlyphs > 0)
  {
    // Calculate the starting position -- offset by ascent (i.e. to baseline).
    const float x0 = m_boxSize.x * (m_origin.x - .5f);
    Vector3 cursor = Vector3(x0, m_boxSize.y * (.5f + m_origin.y), 0.f);
    switch (m_verticalAlignment)
    {
    case Alignment::Centered:
      cursor.y -= (m_boxSize.y - height) * .5f;
      break;

    case Alignment::Positive:
      cursor.y -= (m_boxSize.y - height);
      break;

    default:
      break;
    }

    const auto hAligner = kHorizontalAligners[static_cast<int>(m_horizontalAlignment)];

    cursor.y -= font.GetAscent() * m_scale;

    // Build mesh.
    Font::CachedGlyph const* cg;
    for (auto const& line : m.lines)
    {
      hAligner(m_boxSize.x - line.width, cursor.x);
      maxLineWidth = std::max(maxLineWidth, line.width);

      auto i = line.start;
      XR_ASSERT(BoxText, utf8::is_valid(line.start, line.end));
      while (i != line.end)
      {
        auto codePoint = utf8u::peek_next(i);
        cg = font.CacheGlyph(codePoint);
        if (cg)
        {
          XR_ASSERT(BoxText, positions < posEnd);
          if (cg->uvs.right > 0.f)
          {
            // set pos, uv
            const float gx = cursor.x + cg->glyph->xBearing * m_scale;
            const float gy = cursor.y + cg->glyph->yBearing * m_scale;
            const float gw = cg->glyph->width * m_scale;
            const float gh = cg->glyph->height * m_scale;

            *positions = Vector3(gx, gy + gh, 0.f);
            *uvs = Vector2(cg->uvs.left, cg->uvs.top);
            positions = XR_ADVANCE_PTR(positions, attribStride);
            uvs = XR_ADVANCE_PTR(uvs, attribStride);

            *positions = Vector3(gx, gy, 0.f);
            *uvs = Vector2(cg->uvs.left, cg->uvs.bottom);
            positions = XR_ADVANCE_PTR(positions, attribStride);
            uvs = XR_ADVANCE_PTR(uvs, attribStride);

            *positions = Vector3(gx + gw, gy + gh, 0.f);
            *uvs = Vector2(cg->uvs.right, cg->uvs.top);
            positions = XR_ADVANCE_PTR(positions, attribStride);
            uvs = XR_ADVANCE_PTR(uvs, attribStride);

            *positions = Vector3(gx + gw, gy, 0.f);
            *uvs = Vector2(cg->uvs.right, cg->uvs.bottom);
            positions = XR_ADVANCE_PTR(positions, attribStride);
            uvs = XR_ADVANCE_PTR(uvs, attribStride);
          }

          cursor.x += cg->glyph->advance * m_scale;
        }
        cursor.x += m_horizontalSpacing;

        utf8u::advance(i, 1);
      }

      cursor.y -= lineHeight + m_verticalSpacing;
      cursor.x = x0;
    }
  }

  if (updateGlyphCache)
  {
    m_font->UpdateCache();
  }

  if (statsOut)
  {
    *statsOut = Stats{ uint32_t(m.lines.size()), maxLineWidth, height };
  }
}

//==============================================================================
IndexMesh BoxText::CreateMesh(const char* text, bool updateGlyphCache, Stats* statsOut) const
{
  return CreateMesh(text, strlen(text), updateGlyphCache, statsOut);
}

//==============================================================================
IndexMesh BoxText::CreateMesh(const char* text, size_t numBytes, bool updateGlyphCache,
  Stats* statsOut) const
{
  // Prepare measurement
  Measurement m;
  Measure(text, numBytes, m);

  // Allocate buffer, set index pattern
  std::vector<Vertex> vertices(m.numGlyphs * Quad::Vertex::kCount);
  std::vector<uint16_t> indices(Quad::kIndexCount * m.numGlyphs);
  if (indices.data())
  {
    meshutil::SetIndexPattern(Quad::kIndices, Quad::kIndexCount, Quad::Vertex::kCount,
      m.numGlyphs, indices.data());
  }

  Generate(m, vertices.data(), updateGlyphCache, statsOut);

  return IndexMesh::Create(static_cast<uint32_t>(vertices.size()), vertices.data(),
    static_cast<uint32_t>(indices.size()), indices.data());
}

} // xr

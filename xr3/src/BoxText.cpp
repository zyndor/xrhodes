//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/BoxText.hpp"
#include "XR/Sprite.hpp"
#include "utf8/unchecked.h"

namespace XR {

//==============================================================================
BoxText::BoxText()
: m_font(0),
  m_scale(1.0f),
  m_boxSize(.0f, .0f),
  m_horizontalAlignment(Alignment::Center),
  m_verticalAlignment(Alignment::Center)
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
  XR_ASSERT(Text, width >= .0f);
  XR_ASSERT(Text, height >= .0f);

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
namespace utf8u = utf8::unchecked;

void BoxText::Measure(char const* text, Measurement & m) const
{
  const Font& font = *m_font;

  const float lineHeight = font.GetLineHeight();
  Measurement tm = { Vector2::Zero(), 0 };

  // convert maxSize into font units, since that's what glyph metrics are stored in.
  const Vector2 boxSize = m_boxSize / m_scale;

  const float missingGlyphAdvance = .0f;
  float width = .0f;  // of current row, world units
  float widthWordEnd = .0f; // width at the end of the last completed word, world units
  float widthWordStart = .0f; // width at the start of the current word, world units
  bool newLine = false;
  bool firstChar = true;
  char const* wordStart = nullptr;
  char const* wordEnd = nullptr;
  Measurement::Line line = { text };
  auto numBytes = strlen(text);
  auto end = utf8::find_invalid(text, text + numBytes);
  while (text != end)
  {
    bool hasAdvanced = false;
    auto cp = utf8u::peek_next(text);
    if (cp == '\n')  // line end
    {
      line.width = width;
      line.end = text;

      width = .0f;

      wordStart = nullptr;
      newLine = true;

      utf8u::advance(text, 1);  // skip newline
      hasAdvanced = true;
    }
    else
    {
      // Get glyph.
      auto glyph = font.GetGlyph(cp);
      float wGlyph = missingGlyphAdvance;
      if (glyph)
      {
        wGlyph = glyph->advance;
        if (glyph->fieldHeight != 0)
        {
          ++tm.numGlyphs;
        }

        if (firstChar)
        {
          wGlyph -= glyph->xBearing;
          firstChar = false;
        }
      }

      // If glyph is wider than row, terminate processing.
      if (wGlyph > boxSize.x)
      {
        wordStart = nullptr;
        break;
      }

      // Check word boundary.
      bool isSpace = isspace(cp) != 0;
      if ((wordStart != nullptr) == isSpace) // If we have a word started, and hit space, or if have no word started and hit non-whitespace.
      {
        if (wordStart)  // Had word, now finished.
        {
          widthWordEnd = width;
          wordEnd = text;
          wordStart = nullptr;
        }
        else
        {
          widthWordStart = width;
          wordStart = text;
        }
      }

      if (width + wGlyph > boxSize.x) // We've reached the end of the row.
      {
        newLine = true;
        if (wordEnd > line.start) // There was at least one complete word.
        {
          // We're discarding space. Maybe there should be an option to keep it.
          line.width = widthWordEnd;
          line.end = wordEnd;
          if (wordStart)
          {
            width = width - widthWordStart + wGlyph;
          }
          else
          {
            width = wGlyph;
          }
        }
        else // We're either amidst whitespace, or in a word that has spanned the whole row -- we have to linebreak.
        {
          line.width = width;
          line.end = text;
          width = wGlyph;

          if (!isSpace)  // It is a word, the start of which we'll have processed - repoint wordStart to the current position.
          {
            wordStart = text;
          }
        }

        if (!wordStart && isSpace)  // If the line ends in whitespace, skip it.
        {
          utf8u::advance(text, 1);
          hasAdvanced = true;
        }
      }
      else
      {
        width += wGlyph;
      }
    }

    // Process new line
    if (newLine)
    {
      line.width *= m_scale;  // Convert to scaled units.
      tm.size.x = std::max(line.width, tm.size.x);
      tm.lines.push_back(line); // TODO: avoid allocating here -- enforce an API to do it up front (and bail once we're out of space?).

      if (wordStart)
      {
        line.start = wordStart;
      }
      else
      {
        line.start = text;
      }

      tm.size.y += lineHeight;
      if (tm.size.y + lineHeight >= boxSize.y)  // Prevent vertical overflow.
      {
        break;
      }

      newLine = false;
      firstChar = true;
    }

    if (!hasAdvanced)
    {
      utf8u::advance(text, 1);
    }
  }

  // Process last row - if there's enough vertical space and row not empty.
  if (wordStart && tm.size.y + lineHeight <= boxSize.y)
  {
    line.width = width * m_scale;
    tm.size.x = std::max(line.width, tm.size.x);
    line.end = text;
    tm.size.y += lineHeight;
    tm.lines.push_back(line);
  }

  // Convert to scaled units now.
  tm.size.y *= m_scale;

  m = std::move(tm);
}

//==============================================================================
namespace
{
  const std::function<void(float, float&)> horizontalAligners[] = {
    [](float width, float& x) {},
    [](float width, float& x) { x += width * .5f; },
    [](float width, float& x) { x += width; },
  };
}

#define ADVANCE_PTR(p, stride) p = reinterpret_cast<decltype(p)>(reinterpret_cast<uint8_t*>(p) + (stride))

void BoxText::Generate(Measurement const& m, uint32_t attribStride,
  Vector3* positions, Vector2* uvs, Stats* statsOut)
{
  Font& font = *m_font;

  const float lineHeight = font.GetLineHeight() * m_scale;
  float maxLineWidth = .0f;
  float height = m.size.y;

  if (m.numGlyphs > 0)
  {
    // Calculate the starting position -- offset by ascent (i.e. to baseline).
    Vector3 cursor = Vector3::Zero();
    switch (m_verticalAlignment)
    {
    case Alignment::Center:
      cursor.y -= (m_boxSize.y - height) * .5f;
      break;

    case Alignment::Far:
      cursor.y -= (m_boxSize.y - height);
      break;

    default:
      break;
    }

    const auto hAligner = horizontalAligners[static_cast<int>(m_horizontalAlignment)];

    cursor.y -= font.GetAscent() * m_scale;

    // Build mesh.
    Font::CachedGlyph const* cg;
    for (auto const& line : m.lines)
    {
      hAligner(m_boxSize.x - line.width, cursor.x);
      maxLineWidth = std::max(maxLineWidth, line.width);

      auto i = line.start;
      bool firstChar = true;
      XR_ASSERT(Text, utf8::is_valid(line.start, line.end));
      while (i != line.end)
      {
        auto codePoint = utf8u::peek_next(i);
        cg = font.CacheGlyph(codePoint);
        if (cg)
        {
          if (cg->uvs.right > .0f)
          {
            // set pos, uv
            if (firstChar)
            {
              cursor.x -= cg->glyph->xBearing;
              firstChar = false;
            }

            const float gx = cursor.x + cg->glyph->xBearing * m_scale;
            const float gy = cursor.y + cg->glyph->yBearing * m_scale;
            const float gw = cg->glyph->width * m_scale;
            const float gh = cg->glyph->height * m_scale;

            *positions = Vector3(gx, gy + gh, .0f);
            *uvs = Vector2(cg->uvs.left, cg->uvs.top);
            ADVANCE_PTR(positions, attribStride);
            ADVANCE_PTR(uvs, attribStride);

            *positions = Vector3(gx, gy, .0f);
            *uvs = Vector2(cg->uvs.left, cg->uvs.bottom);
            ADVANCE_PTR(positions, attribStride);
            ADVANCE_PTR(uvs, attribStride);

            *positions = Vector3(gx + gw, gy, .0f);
            *uvs = Vector2(cg->uvs.right, cg->uvs.bottom);
            ADVANCE_PTR(positions, attribStride);
            ADVANCE_PTR(uvs, attribStride);

            *positions = Vector3(gx + gw, gy + gh, .0f);
            *uvs = Vector2(cg->uvs.right, cg->uvs.top);
            ADVANCE_PTR(positions, attribStride);
            ADVANCE_PTR(uvs, attribStride);
          }

          cursor.x += cg->glyph->advance * m_scale;
        }

        firstChar = false;
        utf8u::advance(i, 1);
      }

      cursor.y -= lineHeight;
      cursor.x = .0f;
    }
  }

  if (statsOut)
  {
    *statsOut = { uint32_t(m.lines.size()), maxLineWidth, height };
  }
}


} // XR

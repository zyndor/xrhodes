//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Text.hpp"
#include "XR/Sprite.hpp"
#include "utf8/unchecked.h"

namespace XR {

//==============================================================================
namespace utf8u = utf8::unchecked;

void Text::Measure(char const* text, Font::Ptr font, float scale, Vector2 maxSize,
  Measurement & m)
{
  const float lineHeight = font->GetLineHeight();
  Measurement tm = { Vector2::Zero(), 0 };

  // convert maxSize into font units, since that's what glyph metrics are stored in.
  maxSize /= scale;

  const float missingGlyphAdvance = .0f;
  float width = .0f;  // of current row, world units
  float widthWordEnd = .0f; // width at the end of the last completed word, world units
  float widthWordStart = .0f; // width at the start of the current word, world units
  bool newLine = false;
  bool firstChar = true;
  char const* wordStart = nullptr;
  char const* wordEnd = nullptr;
  Line line = { text };
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
      auto glyph = font->GetGlyph(cp);
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
      if (wGlyph > maxSize.x)
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

      if (width + wGlyph > maxSize.x) // We've reached the end of the row.
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
      line.width *= scale;  // Convert to scaled units.
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
      if (tm.size.y + lineHeight >= maxSize.y)  // Prevent vertical overflow.
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
  if (wordStart && tm.size.y + lineHeight <= maxSize.y)
  {
    line.width = width * scale;
    tm.size.x = std::max(line.width, tm.size.x);
    line.end = text;
    tm.size.y += lineHeight;
    tm.lines.push_back(line);
  }

  // Convert to scaled units now.
  tm.size.y *= scale;

  m = std::move(tm);
}

//==============================================================================
namespace
{
  const std::function<void(float, float&)> hAligners[] = {
    [](float width, float& x) {},
    [](float width, float& x) { x += width * .5f; },
    [](float width, float& x) { x += width; },
  };
}

#define ADVANCE_PTR(p, stride) p = reinterpret_cast<decltype(p)>(reinterpret_cast<uint8_t*>(p) + (stride))

void Text::GenerateMesh(Measurement const& m, Font::Ptr font, float scale,
  Vector2 boxSize, Alignment hAlign, Alignment vAlign, uint32_t attribStride,
  Vector3* positions, Vector2* uvs, Stats* statsOut)
{
  const float lineHeight = font->GetLineHeight() * scale;
  float maxLineWidth = .0f;
  float height = m.size.y;

  if (m.numGlyphs > 0)
  {
    // Calculate the starting position -- offset by ascent (i.e. to baseline).
    Vector3 cursor = Vector3::Zero();
    switch (vAlign)
    {
    case Alignment::Center:
      cursor.y -= (boxSize.y - height) * .5f;
      break;

    case Alignment::Far:
      cursor.y -= (boxSize.y - height);
      break;

    default:
      break;
    }

    auto hAligner = hAligners[static_cast<int>(hAlign)];

    cursor.y -= font->GetAscent() * scale;

    // Build mesh.
    Font::CachedGlyph const* cg;
    for (auto const& line : m.lines)
    {
      hAligner(boxSize.x - line.width, cursor.x);
      maxLineWidth = std::max(maxLineWidth, line.width);

      auto i = line.start;
      bool firstChar = true;
      XR_ASSERT(Text, utf8::is_valid(line.start, line.end));
      while (i != line.end)
      {
        auto codePoint = utf8u::peek_next(i);
        cg = font->CacheGlyph(codePoint);
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

            const float gx = cursor.x + cg->glyph->xBearing * scale;
            const float gy = cursor.y + cg->glyph->yBearing * scale;
            const float gw = cg->glyph->width * scale;
            const float gh = cg->glyph->height * scale;

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

          cursor.x += cg->glyph->advance * scale;
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

//==============================================================================
Text::Text()
: IndexMesh(),
  m_font(0),
  m_scale(!.0f),
  m_boxSize(.0f, .0f),
  m_horizontalAlignment(Alignment::Center),
  m_verticalAlignment(Alignment::Center),
  m_text(),
  m_stats{ 0, .0f, .0f }
{}

//==============================================================================
Text::~Text()
{}

//==============================================================================
void Text::SetFont(const Font::Ptr& font)
{
  m_font = font;
}

//==============================================================================
void Text::SetScale(float scale)
{
  m_scale = scale;
}

//==============================================================================
void Text::SetHorizontalAlignment(Alignment ha)
{
  m_horizontalAlignment = ha;
}

//==============================================================================
void Text::SetVerticalAlignment(Alignment va)
{
  m_verticalAlignment = va;
}

//==============================================================================
void Text::SetText(const char* text)
{
  XR_ASSERT(Text, text != 0);
  m_text = text;
}

//==============================================================================
void  Text::SetBoxSize(float width, float height)
{
  XR_ASSERT(Text, width >= .0f);
  XR_ASSERT(Text, height >= .0f);

  m_boxSize = Vector2(width, height);
}

//==============================================================================
void Text::Update()
{
  // Prepare measurement
  Measurement m;
  Measure(m_text.c_str(), m_font, m_scale, m_boxSize, m);

  // Allocate buffer, set index pattern
  AllocBuffer(m.numGlyphs * Sprite::kNumVertices);
  SetIndexPattern(Sprite::karIndices, Sprite::kNumIndices, Sprite::kNumVertices,
    m.numGlyphs);

  GenerateMesh(m, m_font, m_scale, m_boxSize, m_horizontalAlignment, m_verticalAlignment, GetVertices(),
    &m_stats);

  // Create GPU objects.
  CreateVbo();
  CreateIbo();
}

//==============================================================================
void Text::Clear()
{
  m_text = "";

  AllocBuffer(0);
  SetIndexPattern(nullptr, 0, 0);

  m_stats = { 0, .0f, .0f };
}

} // XR

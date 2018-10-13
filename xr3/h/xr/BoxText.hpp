#ifndef XR_BOXTEXT_HPP
#define XR_BOXTEXT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Font.hpp"
#include "Vertex.hpp"
#include "Quad.hpp"
#include <string>
#include <list>

namespace xr
{

class IndexMesh;

//==============================================================================
///@brief Holds the parameters of a text block that is to be presented line by
/// line, horizontally in a rectangular area of given size, and performs the
/// creation of the text mesh using the given Font.
class BoxText
{
public:
  // types
  using Vertex = Vertex::Format<Vertex::Pos<Vector3>, Vertex::UV0<Vector2>>;

  ///@brief Describes the alignment of text in terms of the relation between its
  /// starting point (in its box) and the origin, along the given axis.
  enum class Alignment
  {
    Near, // Left / Top
    Center,
    Far,  // Right / Bottom
  };

  ///@brief The result of preprocessing a string of text. Specifies the size of
  /// the text block, the number of printable glyphs and the text broken into lines.
  ///@note Does not own its pointers; must not outlive the underlying text buffer.
  struct Measurement
  {
    ///@brief A line of text, with width information to support horizontal alignment.
    ///@note Does not own its pointers; must not outlive the underlying text buffer.
    struct Line
    {
      char const* start;
      char const* end;
      float width;  // scaled
    };

    Vector2 size;
    uint32_t numGlyphs; // printable only; x4 vertices required.
    std::vector<Line> lines;
  };

  ///@brief Byproduct of the mesh generation, carrying the ultimate size (which is
  /// between the topmost, leftmost, rightmost, bottommost vertex positions, ignoring
  /// alignment) and the number of lines [that fit into the target rectangle].
  struct Stats
  {
    uint32_t numLines;
    float maxLineWidth;
    float height;
  };

  // structors
  BoxText();
  ~BoxText();

  // general
  Font::Ptr     GetFont() const;
  float         GetScale() const;
  Vector2       GetBoxSize() const;
  Alignment     GetHorizontalAlignment() const;
  Alignment     GetVerticalAlignment() const;

  void          SetFont(const Font::Ptr& font);
  void          SetScale(float scale);
  void          SetBoxSize(float width, float hight);
  void          SetHorizontalAlignment(Alignment a);
  void          SetVerticalAlignment(Alignment a);

  ///@brief Pre-processes UTF-8 encoded @a text, attempting to fit it into a
  /// rectangle of @a boxSize size (after applying scale), breaking the text
  /// into lines, at word boundaries (if one was found). The output is
  /// the number of glyphs and a vector of Lines, written into @a m.
  void Measure(char const* text, Measurement& m) const;

  ///@brief Generates the mesh for the given text.
  ///@note Expects to have at least m.numGlyphs * 4 elements in the @a positions
  /// and @a uvs arrays.
  void Generate(Measurement const& m, uint32_t attribStride,
    Vector3* positions, Vector2* uvs, bool updateGlyphCache, Stats* statsOut);

  ///@brief Convenience function to generate the mesh for the given text into
  /// the given array of vertices in the given vertex format. Currently
  /// restricted to using UV stream 0.
  /// If @a updateGlyphCache is set, it will update the glyph cache of the font.
  ///@note Expects to have at least m.numGlyps * 4 elements in the @a verts
  /// array.
  template <class VertexFormat>
  inline
  void Generate(Measurement const& m, VertexFormat* verts, bool updateGlyphCache,
    Stats* statsOut)
  {
    Generate(m, VertexFormat::kSize, &verts->pos, &verts->uv0, updateGlyphCache,
      statsOut);
  }

  ///@brief Convenience function to create an IndexMesh with the given text.
  /// If @a updateGlyphCache is set, it will update the glyph cache of the font.
  IndexMesh CreateMesh(const char* text, bool updateGlyphCache, Stats* statsOut);

protected:
  // data
  Font::Ptr     m_font;
  float         m_scale;
  Vector2       m_boxSize;
  Alignment     m_horizontalAlignment;
  Alignment     m_verticalAlignment;
};

//==============================================================================
//  implementation
//==============================================================================
inline
Font::Ptr BoxText::GetFont() const
{
  return m_font;
}

//==============================================================================
inline
float BoxText::GetScale() const
{
  return m_scale;
}

//==============================================================================
inline
Vector2 BoxText::GetBoxSize() const
{
  return m_boxSize;
}

//==============================================================================
inline
BoxText::Alignment  BoxText::GetHorizontalAlignment() const
{
  return m_horizontalAlignment;
}

//==============================================================================
inline
BoxText::Alignment  BoxText::GetVerticalAlignment() const
{
  return m_verticalAlignment;
}

} // XR

#endif // XR_BOXTEXT_HPP

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
/// line, horizontally in a center-aligned rectangular area of given size, and
/// performs the creation of the text mesh using the given Font.
class BoxText
{
public:
  // types
  using Vertex = Vertex::Format<Vertex::Pos<Vector3>, Vertex::UV0<Vector2>>;

  ///@brief Describes the alignment of text in terms of the relation between its
  /// starting point (in its box) and the origin, along the given axis.
  ///@note The conceptual Y axis is reversed - vertical Positive means at the bottom,
  /// however Y still increases upwards.
  enum class Alignment
  {
    Negative,
    Centered,
    Positive,

    // Deprecated
    Near = Negative, // Deprecated; Left / Top
    Center = Centered, // Deprecated
    Far = Positive,  // Deprecated; Right / Bottom
  };

  ///@brief The result of preprocessing a string of text. Specifies the size of
  /// the text block, the number of printable glyphs and the text broken into lines.
  ///@note Does not own its pointers; must not outlive the underlying text buffer.
  struct Measurement
  {
    ///@brief A line of text, with width information to support horizontal (re-)alignment.
    ///@note Does not own its pointers; must not outlive the underlying text buffer.
    struct Line
    {
      char const* start;
      char const* end;
      float width;  // scaled
    };

    Vector2 size; // scaled
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
  Vector2       GetOrigin() const;
  float         GetHorizontalSpacing() const;
  float         GetVerticalSpacing() const;

  ///@brief Sets @a font as current.
  void  SetFont(const Font::Ptr& font);

  ///@brief Sets the scale that is applied to each glyph.
  ///@note Does not affect the size of the box.
  void  SetScale(float scale);

  ///@brief Sets the size of the box.
  ///@note Does not affect scaling.
  void  SetBoxSize(float width, float height);

  ///@brief Sets the horizontal alignment of the text.
  void  SetHorizontalAlignment(Alignment a);

  ///@brief Sets the vertical alignment of the text.
  void  SetVerticalAlignment(Alignment a);

  ///@brief Sets the horizontal spacing, in world units (i.e. as box size.)
  ///@note Spacing is affected by scale.
  void SetHorizontalSpacing(float hs);

  ///@brief Sets the vertical spacing, in world units (i.e. as box size.).
  ///@note Spacing is affected by scale.
  void SetVerticalSpacing(float vs);

  ///@brief Sets the origin in terms of the size of the box. I.e. an origin
  /// of (.5, -.5) means that the pivot of the box is the top left corner.
  // (0., 0.) is the center; this is the default.
  void  SetOrigin(Vector2 origin);

  ///@brief Increases the box height (only if smaller), to ensure that it's
  /// sufficient to fit @a numLines lines of text.
  ///@note A font must be set.
  void EnsureVerticalFit(uint32_t numLines = 1);

  ///@brief Pre-processes UTF-8 encoded @a text to the first null terminator,
  /// attempting to fit it into a rectangle whose size is boxSize (after applying
  /// scale), breaking the text into lines at word boundaries (if one was found,
  /// otherwise at current character). The output is the number of glyphs and a
  /// vector of Lines, written into @a m.
  void Measure(char const* text, Measurement& m) const;

  ///@brief Pre-processes @a numBytes bytes of UTF-8 encoded @a text, attempting
  /// to fit it into a rectangle whose size is boxSize (after applying scale),
  /// breaking the text into lines at word boundaries (if one was found, otherwise
  /// at current character). The output is the number of glyphs and a vector of Lines,
  /// written into @a m.
  void Measure(char const* text, size_t numBytes, Measurement& m) const;

  ///@brief Calculates vertex positions and UVs for the given Measurement.
  ///@param m measurement with text.
  ///@param attribStride byte offset from one position / uv element to the next;
  /// must be the same for both positions / uvs.
  ///@param positions Array of positions to write to.
  ///@param uvs Array of texture coordinates to write to.
  ///@param updateGlyphCache whether glyph cache update should be performed, which
  /// could be false when processing multiple text blocks with the same font.
  ///@param stats Optional Stats object to write to.
  ///@note Expects to have at least m.numGlyphs * 4 elements in the @a positions
  /// and @a uvs arrays.
  void Generate(Measurement const& m, uint32_t attribStride, Vector3* positions,
    Vector2* uvs, bool updateGlyphCache, Stats* statsOut) const;

  ///@brief Convenience function to calculate vertex positions and UVs for the
  /// given Measurement into the given array of vertices, in the given vertex format
  /// (which must have Vector3 position and Vector2 uv). Currently restricted to
  /// using UV stream 0. If @a updateGlyphCache is set, it will update the glyph
  /// cache of the font.
  ///@note Expects to have at least m.numGlyps * 4 elements in the @a verts array.
  template <class VertexFormat>
  inline
  void Generate(Measurement const& m, VertexFormat* verts, bool updateGlyphCache,
    Stats* statsOut) const
  {
    Generate(m, VertexFormat::kSize, &verts->pos, &verts->uv0, updateGlyphCache,
      statsOut);
  }

  ///@brief Convenience function to create an IndexMesh with the given text, up
  /// to the first null terminator.
  /// If @a updateGlyphCache is set, it will update the glyph cache of the font.
  IndexMesh CreateMesh(const char* text, bool updateGlyphCache, Stats* statsOut) const;

  ///@brief Convenience function to create an IndexMesh with @a numBytes bytes
  /// of the given text. If @a updateGlyphCache is set, it will update the glyph
  /// cache of the font.
  IndexMesh CreateMesh(const char* text, size_t numBytes, bool updateGlyphCache,
    Stats* statsOut) const;

protected:
  // data
  Font::Ptr m_font;
  float m_scale;
  Vector2 m_boxSize;
  Alignment m_horizontalAlignment;
  Alignment m_verticalAlignment;

  Vector2 m_origin;
  float m_horizontalSpacing;
  float m_verticalSpacing;
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

//==============================================================================
inline
Vector2 BoxText::GetOrigin() const
{
  return m_origin;
}

//==============================================================================
inline
float BoxText::GetHorizontalSpacing() const
{
  return m_horizontalSpacing;
}

//==============================================================================
inline
float BoxText::GetVerticalSpacing() const
{
  return m_verticalSpacing;
}

} // XR

#endif // XR_BOXTEXT_HPP

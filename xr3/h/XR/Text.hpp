#ifndef XR_TEXT_HPP
#define XR_TEXT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Font.hpp"
#include "IndexMesh.hpp"
#include "Vertex.hpp"
#include <string>
#include <list>

namespace XR {

//==============================================================================
using TextVertexFormat =
  Vertex::Format<Vertex::Pos<Vector3>, Vertex::UV0<Vector2>>;

///@brief Offers capabilities to generate a Text mesh given a font, a scale and
/// a rectangle size.
class Text: public IndexMesh<TextVertexFormat>
{
public:
  // types
  enum  HAlign
  {
    HA_LEFT,
    HA_CENTER,
    HA_RIGHT
  };

  enum  VAlign
  {
    VA_TOP,
    VA_MIDDLE,
    VA_BOTTOM
  };

  ///@brief A line of text, with width information to support horizontal alignment.
  ///@note Does not own its pointers; must not outlive the underlying text buffer.
  struct Line
  {
    char const* start;
    char const* end;
    float width;  // scaled
  };

  ///@brief The result of preprocessing a string of text. Specifies the size of
  /// the text block, the number of printable glyphs and the text broken into lines.
  ///@note Does not own its pointers; must not outlive the underlying text buffer.
  struct Measurement
  {
    Vector2 size;
    size_t numGlyphs; // printable only; x4 vertices required.
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

  // static
  ///@brief Pre-processes UTF-8 encoded @a text, attempting to fit it into a
  /// rectangle of @a maxSize size, after applying @a scale, breaking the text
  /// into lines, at word boundaries (if one was found). The output is
  /// the number of glyphs and a vector of lines, written into @a m.
  static void Measure(char const* text, Font::Ptr font, float scale,
    Vector2 maxSize, Measurement& m);

  ///@brief Generates the mesh for the given text.
  ///@note Expects to have at least m.numGlyphs * 4 elements in the @a positions
  /// and @a uvs arrays.
  static void GenerateMesh(Measurement const& m, Font::Ptr font, float scale,
    Vector2 maxSize, HAlign hAlign, VAlign vAlign, uint32_t attribStride,
    Vector3* positions, Vector2* uvs, Stats* statsOut);

  ///@brief Convenience function to geenrate the mesh for the given text into
  /// the given array of vertices in the given vertex format. Currently
  /// restricted to using UV stream 0.
  ///@note Expects to have at least m.numGlyps * 4 elements in the @a verts
  /// array.
  template <class VertexFormat>
  static void GenerateMesh(Measurement const& m, Font::Ptr font, float scale,
    Vector2 maxSize, HAlign hAlign, VAlign vAlign, VertexFormat* verts, Stats* statsOut)
  {
    GenerateMesh(m, font, scale, maxSize, hAlign, vAlign, VertexFormat::kSize,
      &verts->pos, &verts->uv0, statsOut);
  }

  // structors
  Text();
  ~Text();

  // general
  Font::Ptr     GetFont() const;
  float         GetScale() const;
  Vector2       GetBoxSize() const;
  HAlign        GetHorizontalAlignment() const;
  VAlign        GetVerticalAlignment() const;

  void          SetFont(const Font::Ptr& font, bool update = true);
  void          SetScale(float scale);
  void          SetBoxSize(float boxWidth, float boxHeight, bool update = true);
  void          SetHorizontalAlignment(HAlign ha, bool update = true);
  void          SetVerticalAlignment(VAlign va, bool update = true);
  void          SetText(const char* pText, bool update = true);

  ///@brief Create the mesh based on the given font, scale, box size, alignments.
  ///@note No check made for [lack of] changes - a full blown recreation of text
  /// each time.
  void          Update();

  Stats const& GetStats() const;

  ///@brief Erases the text, destroys and recreates the vertex and index
  /// buffers as empty, and zeroes out stats.
  void          Clear();

protected:
  // data
  Font::Ptr     m_font;
  float         m_scale;
  Vector2       m_boxSize;
  HAlign        m_hAlign;
  VAlign        m_vAlign;
  std::string   m_text;

  Stats m_stats;
};

//==============================================================================
//  implementation
//==============================================================================
inline
Font::Ptr Text::GetFont() const
{
  return m_font;
}

//==============================================================================
inline
float Text::GetScale() const
{
  return m_scale;
}

//==============================================================================
inline
Vector2 Text::GetBoxSize() const
{
  return m_boxSize;
}

//==============================================================================
inline
Text::HAlign  Text::GetHorizontalAlignment() const
{
  return m_hAlign;
}

//==============================================================================
inline
Text::VAlign  Text::GetVerticalAlignment() const
{
  return m_vAlign;
}

//==============================================================================
inline
Text::Stats const& Text::GetStats() const
{
  return m_stats;
}

} // XR

#endif // XR_TEXT_HPP

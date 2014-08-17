//
// Nuclear Heart Games
// XRhodes
//
// Text.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    04/02/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_TEXT_HPP
#define XR_TEXT_HPP

#include <string>
#include "Font.hpp"
#include "IndexMesh.hpp"

namespace XR {

//==============================================================================
class Text: public IndexMesh
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
  
  // structors
  Text();
  ~Text();
  
  // general
  const Font* GetFont() const;
  HAlign      GetHorizontalAlignment() const;
  VAlign      GetVerticalAlignment() const;
  float       GetBoxWidth() const;
  float       GetBoxHeight() const;

  void        SetFont(const Font& f, bool update = true);
  void        SetHorizontalAlignment(HAlign ha, bool update = true);
  void        SetVerticalAlignment(VAlign va, bool update = true);
  void        SetText(const char* pText, bool update = true);
  void        SetSize(float boxWidth, float boxHeight, bool update = true);
  void        Update();

  int         GetNumLines() const;
  float       GetLineWidth(int n) const;
  float       GetMaxLineWidth() const;
  float       GetHeight() const;
  
  void        Clear();
  
protected:
  // types
  struct  Line
  {
    std::string text;
    float       width;
  };
  
  typedef std::list<Line>     LineList;
  typedef std::vector<float>  FloatArray;
  
  // data
  const Font*   m_pFont;
  HAlign        m_hAlign;
  VAlign        m_vAlign;

  float         m_boxWidth;
  float         m_boxHeight;
  std::string   m_text;
  
  FloatArray    m_lines;
  float         m_height;
};

//==============================================================================
//  implementation
//==============================================================================
inline
  const Font* Text::GetFont() const
{
  return m_pFont;
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
float Text::GetBoxWidth() const
{
  return m_boxWidth;
}

//==============================================================================
inline
float Text::GetBoxHeight() const
{
  return m_boxHeight;
}

//==============================================================================
inline
int Text::GetNumLines() const
{
  return (int)m_lines.size();
}

//==============================================================================
inline
float Text::GetLineWidth(int n) const
{
  XR_ASSERT(Text, n >= 0);
  XR_ASSERT(Text, (size_t)n < m_lines.size());
  return m_lines[n];
}

//==============================================================================
inline
float Text::GetHeight() const
{
  return m_height;
}

} // XR

#endif // XR_TEXT_HPP

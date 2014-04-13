//
// Nuclear Heart Games
// XRhodes
//
// UILabel.hpp
// @author  Gyorgy Straub
// @date    21/06/2012
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UILABEL_HPP
#define XR_UILABEL_HPP

#include "UIColoredElement.hpp"
#include <XR/Text.hpp>

namespace XR
{

//==============================================================================
class UILabel: public UIColoredElement
{
public:
  // static
  static const Font* s_pDefaultFont;

  // structors
  UILabel();
  virtual ~UILabel();

  // general
  const Font* GetFont() const;
  int         GetNumLines() const;

  void  SetFont(const Font* pFont);

  Text::HAlign  GetAlignHor() const;
  Text::VAlign  GetAlignVer() const;

  void  SetHorizontalAlignment(Text::HAlign val);
  void  SetVerticalAlignment(Text::VAlign val);

  int   GetLineWidth(int line) const;
  
  void  SetWidthToText();
  void  SetHeightToText();
  void  SetSizeToText();

  void  PrepareText(const char* pText);

  virtual void OnChange();
  
  virtual void Render() const;
  virtual void Render(UIRenderer* pRenderer) const;

protected:
  // data
  mutable Text  m_text;

  // internal
  void  _SetWidthToText();
  void  _SetHeightToText();
};

//==============================================================================
// implementation
//==============================================================================
inline
const Font* UILabel::GetFont() const
{
  return m_text.GetFont();
}

//==============================================================================
inline
Text::HAlign  UILabel::GetAlignHor() const
{
  return m_text.GetHorizontalAlignment();
}

//==============================================================================
inline
Text::VAlign  UILabel::GetAlignVer() const
{
  return m_text.GetVerticalAlignment();
}

//==============================================================================
inline
int UILabel::GetNumLines() const
{
  return m_text.GetNumLines();
}

} // XR

#endif // XR_UILABEL_HPP

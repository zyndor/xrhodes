#ifndef XR_UILABEL_HPP
#define XR_UILABEL_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIColoredElement.hpp"
#include "XR/Text.hpp"

namespace XR
{

//==============================================================================
class UILabel: public UIColoredElement
{
public:
  // static
  static Font::Ptr s_defaultFont;

  // structors
  UILabel();
  virtual ~UILabel();

  // general
  Font::Ptr GetFont() const;
  void  SetFont(Font::Ptr const& pFont);

  Text::HAlign  GetAlignHor() const;
  Text::VAlign  GetAlignVer() const;

  void  SetHorizontalAlignment(Text::HAlign val);
  void  SetVerticalAlignment(Text::VAlign val);

  void  SetWidthToText();
  void  SetHeightToText();
  void  SetSizeToText();

  void  PrepareText(const char* pText);

  const Text& GetText() const;

  virtual void OnChange();

  virtual void Render(IUIRenderer& renderer) const;

protected:
  // data
  mutable Text  m_text;
  int16_t       m_oldWidth;
  int16_t       m_oldHeight;

  // internal
  void Render() const;  // deprecate
};

//==============================================================================
// implementation
//==============================================================================
inline
Font::Ptr UILabel::GetFont() const
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
const Text& UILabel::GetText() const
{
  return m_text;
}

} // XR

#endif // XR_UILABEL_HPP

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
#include "xr/IndexMesh.hpp"
#include "xr/BoxText.hpp"

namespace xr
{

//==============================================================================
///@brief UIElement representing a piece of text.
class UILabel: public UIColoredElement
{
public:
  // static
  static Font::Ptr s_defaultFont;

  // data
  uint32_t renderState; // TODO: UIBuilder support
  Shader::Ptr shader; // TODO: UIBuilder support

  // structors
  UILabel();

  // general
  Font::Ptr GetFont() const;
  BoxText::Alignment  GetHorizontalAlignment() const;
  BoxText::Alignment  GetVerticalAlignment() const;
  float GetScale() const;
  char const* GetText() const;

  void  SetFont(Font::Ptr const& font);
  void  SetHorizontalAlignment(BoxText::Alignment a);
  void  SetVerticalAlignment(BoxText::Alignment a);
  void  SetScale(float scale);
  void  SetText(const char* text);

  void  SetWidthToText();
  void  SetHeightToText();
  void  SetSizeToText();

  void UpdateText();

  void OnChange() override;

  void Render(IUIRenderer& renderer) const override;

protected:
  // data
  IndexMesh m_textMesh;
  BoxText  m_textParams;
  bool m_textDirty;
  std::string m_text;
  BoxText::Stats m_textStats;

  int16_t  m_oldWidth;
  int16_t  m_oldHeight;

  // internal
  void Render() const;  // deprecate
};

//==============================================================================
// implementation
//==============================================================================
inline
Font::Ptr UILabel::GetFont() const
{
  return m_textParams.GetFont();
}

//==============================================================================
inline
BoxText::Alignment  UILabel::GetHorizontalAlignment() const
{
  return m_textParams.GetHorizontalAlignment();
}

//==============================================================================
inline
BoxText::Alignment  UILabel::GetVerticalAlignment() const
{
  return m_textParams.GetVerticalAlignment();
}

//==============================================================================
inline
float UILabel::GetScale() const
{
  return m_textParams.GetScale();
}

////==============================================================================
inline
char const* UILabel::GetText() const
{
  return m_text.c_str();
}

////==============================================================================
//inline
//const BoxText& UILabel::GetText() const
//{
//  return m_textParams;
//}

} // xr

#endif // XR_UILABEL_HPP

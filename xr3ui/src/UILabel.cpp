//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UILabel.hpp"
#include "XR/UIRenderer.hpp"
#include <cstring>
#include <cstdlib>

namespace XR
{

//==============================================================================
Font::Ptr UILabel::s_defaultFont;

//==============================================================================
UILabel::UILabel()
: UIColoredElement(),
  m_text(),
  m_oldWidth(0),
  m_oldHeight(0)
{
  if (s_defaultFont)
  {
    m_text.SetFont(s_defaultFont);
  }
}

//==============================================================================
UILabel::~UILabel()
{}

//==============================================================================
void UILabel::Render() const
{
  XR_ASSERT(UILabel, m_text.GetFont());
  m_text.GetFont()->GetMaterial()->Apply();

  Renderer::SetAmbientColor(color);

  static Matrix m;
  Renderer::GetModelMatrix(m);
  Renderer::SetModelMatrix(Matrix(Vector3(float(x + w / 2), float(y + h / 2), .0f)));
  m_text.RenderOnly(PrimType::TRI_LIST);
  Renderer::SetModelMatrix(m);
}

//==============================================================================
void UILabel::Render(IUIRenderer& renderer) const
{
  renderer.Render();
  Render();
}

//==============================================================================
void UILabel::SetFont(Font::Ptr const& font)
{
  if (font)
  {
    Text::Updater(m_text).SetFont(font);  // TODO: consider rendering text to texture.
  }
}

//==============================================================================
void UILabel::SetHorizontalAlignment(Text::HAlign val )
{
  if (val != m_text.GetHorizontalAlignment())
  {
    m_text.SetHorizontalAlignment(val);
    OnChange();
  }
}

//==============================================================================
void UILabel::SetVerticalAlignment(Text::VAlign val )
{
  if (val != m_text.GetVerticalAlignment())
  {
    m_text.SetVerticalAlignment(val);
    OnChange();
  }
}

//==============================================================================
void UILabel::PrepareText( const char* pText )
{
  XR_ASSERT(UILabel, m_text.GetFont());
  XR_ASSERTMSG(UILabel, pText != 0,
    ("Can't prepare NULL text."));
  Text::Updater(m_text).SetBoxSize((float)w, (float)h)
    .SetText(pText);
}

//==============================================================================
void UILabel::SetWidthToText()
{
  w = int32_t(std::ceilf(m_text.GetStats().maxLineWidth));
}

//==============================================================================
void UILabel::SetHeightToText()
{
  h = int32_t(std::ceilf(m_text.GetStats().height));
}

//==============================================================================
void UILabel::SetSizeToText()
{
  SetWidthToText();
  SetHeightToText();
}

//==============================================================================
void UILabel::OnChange()
{
  if(!(m_oldWidth == w && m_oldHeight == h))
  {
    Text::Updater(m_text).SetBoxSize(float(w), float(h));

    m_oldWidth = w;
    m_oldHeight = h;
  }
}

} // XR

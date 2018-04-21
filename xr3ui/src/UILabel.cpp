//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UILabel.hpp"
#include "XR/UIRenderer.hpp"
#include "XR/Transforms.hpp"
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

  Matrix model(Vector3(float(x + w / 2), float(y + h / 2), .0f));
  Transforms::ScopedModel m(model);
  m_text.RenderOnly(Primitive::TriList);
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
void UILabel::SetHorizontalAlignment(Text::Alignment a)
{
  if (a != m_text.GetHorizontalAlignment())
  {
    m_text.SetHorizontalAlignment(a);
    OnChange();
  }
}

//==============================================================================
void UILabel::SetVerticalAlignment(Text::Alignment a)
{
  if (a != m_text.GetVerticalAlignment())
  {
    m_text.SetVerticalAlignment(a);
    OnChange();
  }
}

//==============================================================================
void UILabel::PrepareText( const char* text )
{
  XR_ASSERT(UILabel, m_text.GetFont());
  XR_ASSERTMSG(UILabel, text != nullptr, ("Can't prepare NULL text."));
  Text::Updater(m_text).SetBoxSize((float)w, (float)h)
    .SetText(text);
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

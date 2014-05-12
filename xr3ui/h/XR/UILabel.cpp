#include <malloc.h>
#include <string.h>
#include "UILabel.hpp"
#include "UIRenderer.hpp"

namespace XR
{

//==============================================================================
const Font* UILabel::s_pDefaultFont(0);

//==============================================================================
UILabel::UILabel()
: UIColoredElement(),
  m_text()
{
  if (s_pDefaultFont != 0)
  {
    m_text.SetFont(*s_pDefaultFont);
  }
}

//==============================================================================
UILabel::~UILabel()
{}

//==============================================================================
void UILabel::Render() const
{
  XR_ASSERT(UILabel, m_text.GetFont() != 0);
  Renderer::SetMaterial(m_text.GetFont()->GetMaterial());
  Renderer::SetAmbientColor(color);
  m_text.Render();
}

//==============================================================================
void UILabel::Render( UIRenderer* pRenderer ) const
{
  pRenderer->Render();
  Render();
}

//==============================================================================
void UILabel::SetFont( const Font* pFont )
{
  if (pFont != 0)
  {
    m_text.SetFont(*pFont);
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
  XR_ASSERT(UILabel, m_text.GetFont() != 0);
  XR_ASSERTMSG(UILabel, pText != 0,
    ("Can't prepare NULL text."));
  m_text.SetSize((float)w, (float)h, false);
  m_text.SetText(pText);
  m_text.GetVertices().Translate(Vector3((float)(w / 2), -(float)(h / 2), .0f));
}

//==============================================================================
void UILabel::SetWidthToText()
{
  Vector3 offset(-(float)(w / 2), .0f, .0f); // negative
  _SetWidthToText();
  offset.x += (float)(w / 2);
  m_text.GetVertices().Translate(offset);
}

//==============================================================================
void UILabel::SetHeightToText()
{
  Vector3 offset(.0f, (float)(h / 2), .0f); // negative
  _SetHeightToText();
  offset.y += -(float)(h / 2);
  m_text.GetVertices().Translate(offset);
}

//==============================================================================
void UILabel::SetSizeToText()
{
  Vector3 offset(-(float)(w / 2), (float)(h / 2), .0f); // negative
  _SetWidthToText();
  _SetHeightToText();
  offset.x += (float)(w / 2);
  offset.y += -(float)(h / 2);
  m_text.GetVertices().Translate(offset);
}

//==============================================================================
void UILabel::_SetWidthToText()
{
  w = m_text.GetMaxLineWidth();
}

//==============================================================================
void UILabel::_SetHeightToText()
{
  h = m_text.GetHeight();
}

//==============================================================================
int UILabel::GetLineWidth( int line ) const
{
  return m_text.GetLineWidth(line);
}

//==============================================================================
void UILabel::OnChange()
{
  m_text.SetSize((float)w, (float)h);
  m_text.GetVertices().Translate(Vector3(w / 2, -h / 2, .0f));
}

} // XR

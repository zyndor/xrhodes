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
  Renderer::SetColStream();

  static Matrix m;
  Renderer::GetModelMatrix(m);
  Renderer::SetModelMatrix(Matrix(Vector3(float(x + w / 2), float(y + h / 2), .0f)));
  m_text.Render();
  Renderer::SetModelMatrix(m);
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
}

//==============================================================================
void UILabel::SetWidthToText()
{
  w = m_text.GetMaxLineWidth();
}

//==============================================================================
void UILabel::SetHeightToText()
{
  h = m_text.GetHeight();
}

//==============================================================================
void UILabel::SetSizeToText()
{
  SetWidthToText();
  SetHeightToText();
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
}

} // XR

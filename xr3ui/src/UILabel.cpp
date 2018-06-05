//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UILabel.hpp"
#include "XR/UIRenderer.hpp"
#include "XR/Transforms.hpp"
#include "XR/Matrix.hpp"
#include <cstring>
#include <cstdlib>

namespace XR
{

//==============================================================================
Font::Ptr UILabel::s_defaultFont;

//==============================================================================
UILabel::UILabel()
: UIColoredElement(),
  renderState(Gfx::F_STATE_ALPHA_BLEND),
  m_textParams(),
  m_textDirty(false),
  m_oldWidth(0),
  m_oldHeight(0)
{
  if (s_defaultFont)
  {
    m_textParams.SetFont(s_defaultFont);
  }
}

//==============================================================================
UILabel::~UILabel()
{}

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
    m_textParams.SetFont(font);  // TODO: consider rendering text to texture.
    m_textDirty = true;
  }
}

//==============================================================================
void UILabel::SetHorizontalAlignment(BoxText::Alignment a)
{
  if (a != m_textParams.GetHorizontalAlignment())
  {
    m_textParams.SetHorizontalAlignment(a);
    m_textDirty = true;
  }
}

//==============================================================================
void UILabel::SetVerticalAlignment(BoxText::Alignment a)
{
  if (a != m_textParams.GetVerticalAlignment())
  {
    m_textParams.SetVerticalAlignment(a);
    m_textDirty = true;
  }
}

//==============================================================================
void UILabel::SetScale(float scale)
{
  m_textParams.SetScale(scale);
  m_textDirty = true;
}

//==============================================================================
void UILabel::SetText( const char* text )
{
  XR_ASSERT(UILabel, m_textParams.GetFont());
  XR_ASSERTMSG(UILabel, text != nullptr, ("Can't prepare NULL text."));
  m_text = text;
  m_textDirty = true;
}

//==============================================================================
void UILabel::SetWidthToText()
{
  w = int32_t(std::ceilf(m_textStats.maxLineWidth));
}

//==============================================================================
void UILabel::SetHeightToText()
{
  h = int32_t(std::ceilf(m_textStats.height));
}

//==============================================================================
void UILabel::SetSizeToText()
{
  SetWidthToText();
  SetHeightToText();
}

//==============================================================================
void UILabel::UpdateText()
{
  if (m_textDirty)
  {
    m_textParams.UpdateMesh(m_text.c_str(), m_textMesh, true, &m_textStats);
    m_textDirty = false;
  }
}

//==============================================================================
void UILabel::OnChange()
{
  if(!(m_oldWidth == w && m_oldHeight == h))
  {
    m_textParams.SetBoxSize(float(w), float(h));
    m_textDirty = true;

    m_oldWidth = w;
    m_oldHeight = h;
  }
}

//==============================================================================
void UILabel::Render() const
{
  XR_ASSERT(UILabel, m_textParams.GetFont());
  const_cast<UILabel*>(this)->UpdateText();

  Material material(Asset::Descriptor<Material>(0), 0); // TODO: Renderable
  material.OverrideStateFlags(0, renderState);
  material.SetShader(shader);
  material.SetTexture(0, m_textParams.GetFont()->GetTexture());

  Matrix model(Vector3(float(x + w / 2), float(y + h / 2), .0f));
  XR_TRANSFORMS_SCOPED_MODEL(model);
  m_textMesh.Render(Primitive::TriList);
}

} // XR

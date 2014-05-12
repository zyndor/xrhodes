#include "UIGridLayout.hpp"

namespace XR
{

//==============================================================================
int16 UIGridLayout::s_defaultRowSpacing(0);
int16 UIGridLayout::s_defaultColumnSpacing(0);

//==============================================================================
void UIGridLayout::SetDefaultSpacing( int16 spacing )
{
  XR_ASSERT(UIGridLayout, spacing >= 0);
  s_defaultRowSpacing = spacing;
  s_defaultColumnSpacing = spacing;
}

//==============================================================================
void UIGridLayout::SetDefaultRowSpacing( int16 spacing )
{
  XR_ASSERT(UIGridLayout, spacing >= 0);
  s_defaultRowSpacing = spacing;
}

//==============================================================================
void UIGridLayout::SetDefaultColumnSpacing( int16 spacing )
{
  XR_ASSERT(UIGridLayout, spacing >= 0);
  s_defaultColumnSpacing = spacing;
}

//==============================================================================
int16 UIGridLayout::GetDefaultRowSpacing()
{
  return s_defaultRowSpacing;
}

//==============================================================================
int16 UIGridLayout::GetDefaultColumnSpacing()
{
  return s_defaultColumnSpacing;
}

//==============================================================================
UIGridLayout::UIGridLayout()
: UIContainer(),
  m_hAlign(AL_LOW),
  m_vAlign(AL_LOW),
  m_numColumns(0),
  m_numRows(0),
  m_columnSpacing(s_defaultRowSpacing),
  m_rowSpacing(s_defaultRowSpacing)
{}

//==============================================================================
UIGridLayout::~UIGridLayout()
{}

//==============================================================================
void UIGridLayout::_AlignElement( UIElement* pElem )
{
  int column(m_lElements.size() % m_numColumns);
  int row(m_lElements.size() / m_numColumns);

  if (m_hAlign != AL_NOALIGN)
  {
    int xElem(x + column * m_columnSpacing);
    switch (m_hAlign)
    {
    case  AL_LOW:
      break;

    case  AL_CENTER:
      xElem += (m_columnSpacing - pElem->w) / 2;
      break;

    case  AL_HIGH:
      xElem += m_columnSpacing - pElem->w;
      break;
    }
    pElem->x = xElem;
  }

  if (m_vAlign != AL_NOALIGN)
  {
    int yElem(y + row * m_columnSpacing);
    switch (m_vAlign)
    {
    case  AL_LOW:
      break;

    case  AL_CENTER:
      yElem += (m_rowSpacing - pElem->h) / 2;
      break;

    case  AL_HIGH:
      yElem += m_rowSpacing - pElem->h;
      break;
    }
    pElem->y = yElem;
  }

  pElem->OnChange();
}

//==============================================================================
void UIGridLayout::SetColumnSpacing(int16 spacing)
{
  if (spacing != m_columnSpacing)
  {
    m_columnSpacing = spacing;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::SetRowsSpacing(int16 spacing)
{
  if (spacing != m_rowSpacing)
  {
    m_rowSpacing = spacing;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::SetSpacing(int16 spacing)
{
  if (m_columnSpacing != spacing || m_rowSpacing != spacing)
  {
    m_columnSpacing = spacing;
    m_rowSpacing = spacing;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::SetNumRows( int16 rows )
{
  XR_ASSERT(UIGridLayout, rows >= 0);
  if (m_numRows != rows)
  {
    m_numRows = rows;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::SetNumColumns( int16 cols )
{
  XR_ASSERT(UIGridLayout, cols >= 0);
  if (m_numColumns != cols)
  {
    m_numColumns = cols;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::SetGrid( int16 cols, int16 rows )
{
  XR_ASSERT(UIGridLayout, rows >= 0);
  XR_ASSERT(UIGridLayout, cols >= 0);
  if (cols != m_numColumns || rows != m_numRows)
  {
    m_numColumns = cols;
    m_numRows = rows;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::SetHorizontalAlignment( Alignment hAlign )
{
  if (hAlign != m_hAlign)
  {
    m_hAlign = hAlign;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::SetVerticalAlignment( Alignment vAlign )
{
  if (vAlign != m_vAlign)
  {
    m_vAlign = vAlign;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::SetAlignment( Alignment hAlign, Alignment vAlign )
{
  if (hAlign != m_hAlign || vAlign != m_vAlign)
  {
    m_hAlign = hAlign;
    m_vAlign = vAlign;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::_SetWidthToContent()
{
  int wNew(0);
  for (ElementList::const_iterator i0(m_lElements.begin()), i1(m_lElements.end()); i0 != i1; ++i0)
  {
    int wElem((*i0)->w);
    if (wElem > wNew)
    {
      wNew = wElem;
    }
  }

  m_columnSpacing = wNew;
  w = m_columnSpacing * m_numColumns;
}

//==============================================================================
void UIGridLayout::_SetHeightToContent()
{
  int hNew(0);
  for (ElementList::const_iterator i0(m_lElements.begin()), i1(m_lElements.end()); i0 != i1; ++i0)
  {
    int hElem((*i0)->h);
    if (hElem > hNew)
    {
      hNew = hElem;
    }
  }

  m_rowSpacing = hNew;
  h = m_rowSpacing * m_numRows;
}

} // XR

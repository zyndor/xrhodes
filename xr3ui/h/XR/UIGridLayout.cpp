#include "UIGridLayout.hpp"

namespace XR
{

//==============================================================================
int16 UIGridLayout::s_defaultRowPadding(0);
int16 UIGridLayout::s_defaultColumnPadding(0);

//==============================================================================
void UIGridLayout::SetDefaultPadding( int16 padding )
{
  XR_ASSERT(UIGridLayout, padding >= 0);
  s_defaultRowPadding = padding;
  s_defaultColumnPadding = padding;
}

//==============================================================================
void UIGridLayout::SetDefaultRowPadding( int16 padding )
{
  XR_ASSERT(UIGridLayout, padding >= 0);
  s_defaultRowPadding = padding;
}

//==============================================================================
void UIGridLayout::SetDefaultColumnPadding( int16 padding )
{
  XR_ASSERT(UIGridLayout, padding >= 0);
  s_defaultColumnPadding = padding;
}

//==============================================================================
int16 UIGridLayout::GetDefaultRowPadding()
{
  return s_defaultRowPadding;
}

//==============================================================================
int16 UIGridLayout::GetDefaultColumnPadding()
{
  return s_defaultColumnPadding;
}

//==============================================================================
UIGridLayout::UIGridLayout()
: UIContainer(),
  m_hAlign(AL_LOW),
  m_vAlign(AL_LOW),
  m_numColumns(0),
  m_numRows(0),
  m_columnPadding(s_defaultRowPadding),
  m_rowPadding(s_defaultRowPadding)
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
    int xElem(x + column * (m_cellWidth + m_columnPadding));
    switch (m_hAlign)
    {
    case  AL_LOW:
      break;

    case  AL_CENTER:
      xElem += (m_cellWidth - pElem->w) / 2;
      break;

    case  AL_HIGH:
      xElem += m_cellWidth - pElem->w;
      break;
    }
    pElem->x = xElem;
  }

  if (m_vAlign != AL_NOALIGN)
  {
    int yElem(y + row * (m_cellHeight + m_rowPadding));
    switch (m_vAlign)
    {
    case  AL_LOW:
      break;

    case  AL_CENTER:
      yElem += (m_cellHeight - pElem->h) / 2;
      break;

    case  AL_HIGH:
      yElem += m_cellHeight - pElem->h;
      break;
    }
    pElem->y = yElem;
  }

  pElem->OnChange();
}

//==============================================================================
void UIGridLayout::SetCellWidth(int16 wCell)
{
  if (wCell != m_cellWidth)
  {
    m_cellWidth = wCell;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::SetCellHeight(int16 hCell)
{
  if (hCell != m_cellHeight)
  {
    m_cellHeight = hCell;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::SetCellSize(int16 wCell, int16 hCell)
{
  if (wCell != m_cellWidth || hCell != m_cellHeight)
  {
    m_cellWidth = wCell;
    m_cellHeight = hCell;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::SetColumnPadding(int16 padding)
{
  if (padding != m_columnPadding)
  {
    m_columnPadding = padding;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::SetRowPadding(int16 padding)
{
  if (padding != m_rowPadding)
  {
    m_rowPadding = padding;
    OnChange();
  }
}

//==============================================================================
void UIGridLayout::SetCellPadding(int16 xPadding, int16 yPadding)
{
  if (m_columnPadding != xPadding || m_rowPadding != yPadding)
  {
    m_columnPadding = xPadding;
    m_rowPadding = yPadding;
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
  for (UIElement::List::const_iterator i0(m_lElements.begin()), i1(m_lElements.end()); i0 != i1; ++i0)
  {
    int wElem((*i0)->w);
    if (wElem > wNew)
    {
      wNew = wElem;
    }
  }

  m_cellWidth = wNew;
  w = m_cellWidth * m_numColumns + m_columnPadding * (m_numColumns - 1);
}

//==============================================================================
void UIGridLayout::_SetHeightToContent()
{
  int hNew(0);
  for (UIElement::List::const_iterator i0(m_lElements.begin()), i1(m_lElements.end()); i0 != i1; ++i0)
  {
    int hElem((*i0)->h);
    if (hElem > hNew)
    {
      hNew = hElem;
    }
  }

  m_cellHeight = hNew;
  h = m_cellHeight * m_numRows + m_rowPadding * (m_numRows - 1);
}

} // XR

#ifndef XR_UIGRIDLAYOUT_HPP
#define XR_UIGRIDLAYOUT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "UIContainer.hpp"

namespace XR
{

//==============================================================================
///@brief container that's placing the elements
class UIGridLayout: public UIContainer
{
public:
  // static
  static int		GetDefaultRowPadding();
  static int		GetDefaultColumnPadding();

  static void   SetDefaultPadding(int32_t padding);
  static void   SetDefaultRowPadding(int32_t padding);
  static void   SetDefaultColumnPadding(int32_t padding);

  // structors
  UIGridLayout();
  virtual ~UIGridLayout();

  // general
  int GetNumRows() const;
  int GetNumColumns() const;

  int GetCellWidth() const;
  int GetCellHeight() const;

  int GetColumnPadding() const;
  int GetRowPadding() const;

  void  SetCellWidth(int32_t wCell);
  void  SetCellHeight(int32_t hCell);
  void  SetCellSize(int32_t wCell, int32_t hCell);

  void  SetColumnPadding(int32_t padding);
  void  SetRowPadding(int32_t padding);
  void  SetCellPadding(int32_t xPadding, int32_t yPadding);

  void  SetHorizontalAlignment(Alignment hAlign);
  void  SetVerticalAlignment(Alignment vAlign);
  void  SetAlignment(Alignment hAlign, Alignment vAlign);

  void  SetNumRows(int rows);
  void  SetNumColumns(int cols);
  void  SetGrid(int columns, int rows);

protected:
  // static
  static int32_t s_defaultRowPadding;
  static int32_t s_defaultColumnPadding;

  // data
  Alignment m_hAlign;
  Alignment m_vAlign;

  int   m_numColumns;
  int   m_numRows;
  int32_t m_cellWidth;
  int32_t m_cellHeight;
  int32_t m_columnPadding;
  int32_t m_rowPadding;

  // internal
  virtual void _AlignElement( UIElement* pElem );

  virtual void _SetWidthToContent();
  virtual void _SetHeightToContent();
};

//==============================================================================
// implementation
//==============================================================================
inline
int UIGridLayout::GetNumRows() const
{
  return m_numRows;
}

//==============================================================================
inline
int UIGridLayout::GetNumColumns() const
{
  return m_numColumns;
}

//==============================================================================
inline
int UIGridLayout::GetCellWidth() const
{
  return m_cellWidth;
}

//==============================================================================
inline
int UIGridLayout::GetCellHeight() const
{
  return m_cellHeight;
}

//==============================================================================
inline
int UIGridLayout::GetColumnPadding() const
{
  return m_columnPadding;
}

//==============================================================================
inline
int UIGridLayout::GetRowPadding() const
{
  return m_rowPadding;
}

} // XR

#endif // XR_UIGRIDLAYOUT_HPP

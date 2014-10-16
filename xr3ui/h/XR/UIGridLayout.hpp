//
// Nuclear Heart Games
// XRhodes
//
// UIGridLayout.hpp
// @author  Gyorgy Straub
// @date    21/06/2012
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UIGRIDLAYOUT_HPP
#define XR_UIGRIDLAYOUT_HPP

#include "UIContainer.hpp"

namespace XR
{

//==============================================================================
///@brief container that's placing the elements
class UIGridLayout: public UIContainer
{
public:
  // static
  static int16  GetDefaultRowPadding();
  static int16  GetDefaultColumnPadding();

  static void   SetDefaultPadding(int16 padding);
  static void   SetDefaultRowPadding(int16 padding);
  static void   SetDefaultColumnPadding(int16 padding);

  // structors
  UIGridLayout();
  virtual ~UIGridLayout();

  // general
  int16 GetNumRows() const;
  int16 GetNumColumns() const;

  int16 GetCellWidth() const;
  int16 GetCellHeight() const;

  int16 GetColumnPadding() const;
  int16 GetRowPadding() const;

  void  SetCellWidth(int16 wCell);
  void  SetCellHeight(int16 hCell);
  void  SetCellSize(int16 wCell, int16 hCell);

  void  SetColumnPadding(int16 padding);
  void  SetRowPadding(int16 padding);
  void  SetCellPadding(int16 xPadding, int16 yPadding);

  void  SetHorizontalAlignment(Alignment hAlign);
  void  SetVerticalAlignment(Alignment vAlign);
  void  SetAlignment(Alignment hAlign, Alignment vAlign);

  void  SetNumRows(int16 rows);
  void  SetNumColumns(int16 cols);
  void  SetGrid(int16 columns, int16 rows);

protected:
  // static
  static int16  s_defaultRowPadding;
  static int16  s_defaultColumnPadding;

  // data
  Alignment m_hAlign;
  Alignment m_vAlign;

  int   m_numColumns;
  int   m_numRows;
  int16 m_cellWidth;
  int16 m_cellHeight;
  int16 m_columnPadding;
  int16 m_rowPadding;

  // internal
  virtual void _AlignElement( UIElement* pElem );

  virtual void _SetWidthToContent();
  virtual void _SetHeightToContent();
};

//==============================================================================
// implementation
//==============================================================================
inline
int16 UIGridLayout::GetNumRows() const
{
  return m_numRows;
}

//==============================================================================
inline
int16 UIGridLayout::GetNumColumns() const
{
  return m_numColumns;
}

} // XR

#endif // XR_UIGRIDLAYOUT_HPP

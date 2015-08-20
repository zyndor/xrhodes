//
// Nuclear Heart Games
// XRhodes
//
// UIGridLayout.hpp
// @author  Gyorgy Straub
// @date    21/06/2012
//
// copyright (c) 2011 - 2015. All rights reserved.
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
  static int		GetDefaultRowPadding();
  static int		GetDefaultColumnPadding();

  static void   SetDefaultPadding(int padding);
  static void   SetDefaultRowPadding(int padding);
  static void   SetDefaultColumnPadding(int padding);

  // structors
  UIGridLayout();
  virtual ~UIGridLayout();

  // general
  int		GetNumRows() const;
  int		GetNumColumns() const;

  int		GetCellWidth() const;
  int		GetCellHeight() const;

  int		GetColumnPadding() const;
  int		GetRowPadding() const;

  void  SetCellWidth(int wCell);
  void  SetCellHeight(int hCell);
  void  SetCellSize(int wCell, int hCell);

  void  SetColumnPadding(int padding);
  void  SetRowPadding(int padding);
  void  SetCellPadding(int xPadding, int yPadding);

  void  SetHorizontalAlignment(Alignment hAlign);
  void  SetVerticalAlignment(Alignment vAlign);
  void  SetAlignment(Alignment hAlign, Alignment vAlign);

  void  SetNumRows(int rows);
  void  SetNumColumns(int cols);
  void  SetGrid(int columns, int rows);

protected:
  // static
  static int  s_defaultRowPadding;
  static int  s_defaultColumnPadding;

  // data
  Alignment m_hAlign;
  Alignment m_vAlign;

  int   m_numColumns;
  int   m_numRows;
  int m_cellWidth;
  int m_cellHeight;
  int m_columnPadding;
  int m_rowPadding;

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

} // XR

#endif // XR_UIGRIDLAYOUT_HPP

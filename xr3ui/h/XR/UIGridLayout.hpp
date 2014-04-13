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
  static int16  GetDefaultRowSpacing();
  static int16  GetDefaultColumnSpacing();

  static void   SetDefaultSpacing(int16 spacing);
  static void   SetDefaultRowSpacing(int16 spacing);
  static void   SetDefaultColumnSpacing(int16 spacing);

  // structors
  UIGridLayout();
  virtual ~UIGridLayout();

  // general
  int16 GetNumRows() const;
  int16 GetNumColumns() const;

  void  SetColumnSpacing(int16 spacing);
  void  SetRowsSpacing(int16 spacing);
  void  SetSpacing(int16 spacing);

  void  SetHorizontalAlignment(Alignment hAlign);
  void  SetVerticalAlignment(Alignment vAlign);
  void  SetAlignment(Alignment hAlign, Alignment vAlign);

  void  SetNumRows(int16 rows);
  void  SetNumColumns(int16 cols);
  void  SetGrid(int16 columns, int16 rows);

protected:
  // static
  static int16  s_defaultRowSpacing;
  static int16  s_defaultColumnSpacing;

  // data
  Alignment m_hAlign;
  Alignment m_vAlign;

  int16 m_numColumns;
  int16 m_numRows;
  int16 m_columnSpacing;  // ignores size of elements
  int16 m_rowSpacing;  // ignores size of elements

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

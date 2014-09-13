//
// Nuclear Heart Games
// XRhodes
//
// UIGrowingLayoutBase.hpp
// @author  Gyorgy Straub
// @date    21/06/2012
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UIGROWINGLAYOUTBASE_HPP
#define XR_UIGROWINGLAYOUTBASE_HPP

#include "UIContainer.hpp"

namespace XR
{

//==============================================================================
class UIGrowingLayoutBase:  public UIContainer
{
public:
  // types
  enum  GrowDir // along axis, in screen space
  {
    GD_POSITIVE,
    GD_NEGATIVE
  };

  // static
  static void   SetDefaultSpacing(int16 spacing);
  static int16  GetDefaultSpacing();

  // structors
  UIGrowingLayoutBase();
  virtual ~UIGrowingLayoutBase();

  // virtual
  virtual int16 CalculateElementsExtent() const =0;

  // general
  GrowDir   GetGrowDir() const;
  Alignment GetAlignment() const;
  int16     GetSpacing() const;

  void  SetGrowDir(GrowDir growDir);
  void  SetAlignment(Alignment align);
  void  SetSpacing(int16 spacing);

protected:
  // static
  static int16  s_defaultSpacing;

  // data
  GrowDir   m_growDir;
  Alignment m_align;
  int16     m_spacing;
};

//==============================================================================
// implementation
//==============================================================================
inline
UIGrowingLayoutBase::GrowDir UIGrowingLayoutBase::GetGrowDir() const
{
  return m_growDir;
}

//==============================================================================
inline
UIGrowingLayoutBase::Alignment UIGrowingLayoutBase::GetAlignment() const
{
  return m_align;
}

//==============================================================================
inline
int16 UIGrowingLayoutBase::GetSpacing() const
{
  return m_spacing;
}

} // XR

#endif // XR_UIGROWINGLAYOUTBASE_HPP

#ifndef XR_UIGROWINGLAYOUTBASE_HPP
#define XR_UIGROWINGLAYOUTBASE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "UIContainer.hpp"

namespace xr
{

//==============================================================================
///@brief Base class for containers that organize their child elements in one
/// line that grows with every new child.
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
  static void SetDefaultSpacing(int spacing);
  static int  GetDefaultSpacing();

  // structors
  UIGrowingLayoutBase();

  // virtual
  virtual int CalculateElementsExtent() const =0;

  // general
  GrowDir   GetGrowDir() const;
  Alignment GetAlignment() const;
  int     GetSpacing() const;

  void  SetGrowDir(GrowDir growDir);
  void  SetAlignment(Alignment align);
  void  SetSpacing(int spacing);

protected:
  // static
  static int  s_defaultSpacing;

  // data
  GrowDir   m_growDir;
  Alignment m_align;
  int     m_spacing;
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
int UIGrowingLayoutBase::GetSpacing() const
{
  return m_spacing;
}

} // xr

#endif // XR_UIGROWINGLAYOUTBASE_HPP

#ifndef XR_UIHORIZONTALLAYOUT_HPP
#define XR_UIHORIZONTALLAYOUT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "UIGrowingLayoutBase.hpp"

namespace xr
{

//==============================================================================
///@brief Container that places its children side by side in one horizontal line.
class UIHorizontalLayout: public UIGrowingLayoutBase
{
public:
  // structors
  UIHorizontalLayout();

  // general
  int CalculateElementsExtent() const override;

protected:
  // internal
  void AlignChildImpl( UIElement* elem ) override;

  void SetWidthToContentImpl() override;
};

//==============================================================================
// implementation
//==============================================================================
inline
int UIHorizontalLayout::CalculateElementsExtent() const
{
  return m_children.empty() ? 0 : (m_growDir == GD_POSITIVE ?
    (m_children.back()->CalculateRight() - m_children.front()->x) :
    (m_children.front()->CalculateRight() - m_children.back()->x));
}

} // xr

#endif // XR_UIHORIZONTALLAYOUT_HPP

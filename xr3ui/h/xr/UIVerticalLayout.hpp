#ifndef XR_UIVERTICALLAYOUT_HPP
#define XR_UIVERTICALLAYOUT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIGrowingLayoutBase.hpp"

namespace xr
{

//==============================================================================
///@brief Container that places its children top to bottom in one vertical line.
  class UIVerticalLayout: public UIGrowingLayoutBase
{
public:
  // structors
  UIVerticalLayout();

  // general
  int CalculateElementsExtent() const override;

protected:
  // internal
  void AlignChildImpl( UIElement* elem ) override;

  void SetHeightToContentImpl() override;
};

//==============================================================================
// implementation
//==============================================================================
inline
int UIVerticalLayout::CalculateElementsExtent() const
{
  return m_children.empty() ? 0 : (m_growDir == GD_POSITIVE ?
    (m_children.back()->CalculateBottom() - m_children.front()->y) :
    (m_children.front()->CalculateBottom() - m_children.back()->y));
}

} // xr

#endif // XR_UIVERTICALLAYOUT_HPP

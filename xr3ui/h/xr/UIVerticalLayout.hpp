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
class UIVerticalLayout: public UIGrowingLayoutBase
{
public:
  // structors
  UIVerticalLayout();
  ~UIVerticalLayout();

  // general
  virtual int CalculateElementsExtent() const;

protected:
  // internal
  virtual void _AlignElement( UIElement* elem );

  virtual void _SetHeightToContent();
};

//==============================================================================
// implementation
//==============================================================================
inline
int UIVerticalLayout::CalculateElementsExtent() const
{
  return m_elements.empty() ? 0 : (m_growDir == GD_POSITIVE ?
    (m_elements.back()->CalculateBottom() - m_elements.front()->y) :
    (m_elements.front()->CalculateBottom() - m_elements.back()->y));
}

} // xr

#endif // XR_UIVERTICALLAYOUT_HPP
